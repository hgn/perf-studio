#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "perf-studio.h"
#include "shared.h"
#include "project.h"

#define PERF_STUDIO_USER_CONF_DIR "perf-studio"
#define PERF_STUDIO_USER_GLOBAL_CONF_NAME "config"
#define PERF_STUDIO_USER_PROJECT_DIR_NAME "projects"


int load_user_conf_file(struct ps *ps)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	gsize length;
	gchar *full_path;
	gchar **iter;

	full_path = g_build_filename(g_get_user_config_dir(),
				     PERF_STUDIO_USER_CONF_DIR,
				     PERF_STUDIO_USER_GLOBAL_CONF_NAME,
				     NULL);
	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file(keyfile, full_path, flags, NULL)) {
		pr_info(ps, "failed to open user configuration file [%s]", full_path);
		goto out;
	}


	ps->conf.common.perf_path = g_key_file_get_string(keyfile, "common", "perf-path", NULL);
	pr_info(ps, "perf-path: %s", ps->conf.common.perf_path);

	ps->conf.common.username = g_key_file_get_string(keyfile, "common", "username", NULL);
	pr_info(ps, "username: %s", ps->conf.common.username);

	ps->conf.common.module_paths = g_key_file_get_string_list(keyfile, "common", "module-paths", &length, NULL);
	iter = ps->conf.common.module_paths;
	while (iter && *iter) {
		pr_info(ps, "module-paths: %s", *iter);
		iter++;
	}

	/* FIXME: needs parsing */
	ps->conf.ui.statusbar_enabled = FALSE;

out:
	g_free(full_path);
	g_key_file_free(keyfile);
	return 0;
}

#define CONFIG_NAME "config"


/* foo/bar/project-foo/.perf-studio/config */
static struct project *load_new_project(struct ps *ps, GKeyFile *keyfile,
					const char *project_id, const char *path)
{
	gchar *cmd;
	struct project *project;
	gchar *last_used_tmp;

	assert(project_id);

	cmd = g_key_file_get_string(keyfile, "common", "cmd", NULL);
	if (!cmd) {
		pr_error(ps, "Project has no command for execution specified!");
		return NULL;
	}

	/* ok, project seems sane, create it */
	project = project_new();
	project->cmd = cmd;
	project->id = g_strdup(project_id);
	project->project_path = g_strdup(path);

	/* optional arguments */
	project->cmd_args_full = g_key_file_get_string(keyfile, "common", "cmd-args", NULL);
	if (project->cmd_args_full) {
		project->cmd_args_splitted = g_strsplit_set(project->cmd_args_full,  " \t", -1);
	}

	/* optional arguments */
	project->description = g_key_file_get_string(keyfile, "common", "description", NULL);

	/* optional argument */
	last_used_tmp = g_key_file_get_string(keyfile, "stats", "last-used", NULL);
	if (last_used_tmp != NULL) {
		project->last_used_timestamp = g_ascii_strtoull(last_used_tmp, NULL, 10);
	} else {
		project->last_used_timestamp = 0;
	}
	g_free(last_used_tmp);

	project_show(ps, project);

	return project;
}


/*
 * Change the last-used timestamp data.
 * We already have all data from the keyfile. We
 * now save a new file with the exaclty same context - except the
 * timestamp. And finally we call rename to do it atomically
 */
void project_conf_file_update_last_used(struct ps *ps, struct project *project)
{
	int fd;
	gsize length;
	gchar *key_file_content;
	gchar *tmp_name;
	gchar *full_path;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	guint64 current_time;
	char buf[32];
	gchar *project_path;

	assert(project);
	assert(project->project_path);

	project_path = project->project_path;

	full_path = g_build_filename(project_path,
				     PERF_STUDIO_USER_GLOBAL_CONF_NAME,
				     NULL);
	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file(keyfile, full_path, flags, NULL)) {
		pr_info(ps, "failed to open user configuration file [%s]", full_path);
		goto err;
	}

	tmp_name = g_strdup_printf("%s.XXXXXX", "perf-studio-project-conf");

	fd = g_mkstemp_full(tmp_name, O_RDWR, 0666);
	if (fd < 0) {
		pr_warn(ps, "Could not update last-used timestamp");
		goto err2;
	}

	current_time = g_get_real_time();
	snprintf(buf, sizeof(buf) - 1, "%" G_GUINT64_FORMAT, current_time);
	project->last_used_timestamp = current_time;

	g_key_file_set_string(keyfile, "stats", "last-used", buf);

	key_file_content = g_key_file_to_data(keyfile, &length, NULL);
	write(fd, key_file_content, length);
	close(fd);

	g_rename(tmp_name, full_path);

err2:
	g_free(full_path);
	g_free(tmp_name);
	g_free(key_file_content);
	g_key_file_free(keyfile);
err:
	return;
}


#define EXAMPLE_ID "0001"

static void load_check_conf(struct ps *ps, const char *file_name, gchar *project_path)
{
	GFile *file;
	gchar *project_path_name;
	GFileInfo *file_info;
	gboolean bret;
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	struct project *project;


	if (strlen(file_name) != strlen(EXAMPLE_ID)) {
		pr_warn(ps, "Strange project ID (%s) in %s", file_name, project_path);
		return;
	}

	project_path_name = g_build_filename(project_path, PERF_STUDIO_USER_GLOBAL_CONF_NAME, NULL);
	file = g_file_new_for_path(project_path);
	file_info = g_file_query_info(file, G_FILE_ATTRIBUTE_ACCESS_CAN_READ ","
				      G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE, 0, NULL, NULL);
	bret = g_file_info_get_attribute_boolean(file_info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
	if (!bret) {
		pr_warn(ps, "Configuraion file not readable: %s", project_path_name);
		goto out;
	}

	bret = g_file_info_get_attribute_boolean(file_info, G_FILE_ATTRIBUTE_ACCESS_CAN_WRITE);
	if (!bret) {
		pr_warn(ps, "Configuraion file not writeable: %s", project_path_name);
		goto out;
	}

	pr_info(ps, "Try to read configuration file: %s", project_path_name);

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file(keyfile, project_path_name, flags, NULL)) {
		pr_info(ps, "failed to open project configuration file [%s]", project_path_name);
		goto out2;
	}

	/*
	 * Read project configuration values, like cmd
	 * If this is successfull we consider this project
	 * as sane, problems with refs/db are handled properly
	 */
	project = load_new_project(ps, keyfile, file_name, project_path);
	if (!project) {
		pr_error(ps, "Project %s seems corrupt, ignoring it", project_path_name);
		goto out2;
	}

	pr_info(ps, "project seems fine - add to list");
	project_add(ps, project);

out2:
	g_key_file_free(keyfile);
out:
	g_object_unref(file);
	g_free(project_path_name);
}


int load_projects_from_cache(struct ps *ps)
{
	GFile *file;
	gchar *file_path, *n_path;
	GFileEnumerator *enumerator;
	GFileInfo *file_info;
	const char *file_name;
	GFileType file_type;

	file_path = g_build_filename(g_get_user_cache_dir(),
			             PERF_STUDIO_USER_CONF_DIR,
				     PERF_STUDIO_USER_PROJECT_DIR_NAME,
				     NULL);
	file = g_file_new_for_path(file_path);

	enumerator = g_file_enumerate_children(
			file,
			"standard::*",
			G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
			NULL,
			NULL);

	if (enumerator != NULL) {
		while ((file_info = g_file_enumerator_next_file(enumerator, NULL, NULL))) {
			file_name = g_file_info_get_name (file_info);
			file_type = g_file_info_get_file_type (file_info);
			if (file_type == G_FILE_TYPE_DIRECTORY) {
				n_path = g_build_filename(file_path, file_name, NULL);
				load_check_conf(ps, file_name, n_path);
				g_free(n_path);
			}
			g_object_unref(file_info);
		}
		g_object_unref(enumerator);
	}

	g_object_unref(file);
	g_free(file_path);

	return 0;
}
