#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	const gchar *data_dir;
	gchar *full_path;
	gchar **iter;

	data_dir = g_get_user_config_dir();
	full_path = g_malloc(strlen(data_dir) +
			1 + /* "/" */
			strlen(PERF_STUDIO_USER_CONF_DIR) +
			1 + /* "/" */
			strlen(PERF_STUDIO_USER_GLOBAL_CONF_NAME) +
			1); /* "\0" */

	strcpy(full_path, data_dir);
	strcat(full_path, "/");
	strcat(full_path, PERF_STUDIO_USER_CONF_DIR);
	strcat(full_path, "/");
	strcat(full_path, PERF_STUDIO_USER_GLOBAL_CONF_NAME);

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
static struct project *load_new_project(struct ps *ps, GKeyFile *keyfile, const char *path)
{
	gchar *exec_path;
	struct project *project;

	exec_path = g_key_file_get_string(keyfile, "common", "exec-path", NULL);
	if (!exec_path) {
		pr_error(ps, "Project has not command!");
		return NULL;
	}

	/* ok, project seems sane, create it */
	project = project_new();
	project->exec_path       = exec_path;
	project->project_db_path = g_strdup(path);

	/* optional arguments */
	project->exec_args = g_key_file_get_string_list(keyfile, "common", "exec-args", NULL, NULL);

	project_show(ps, project);

	return project;
}


#define EXAMPLE_ID "0001"

static void load_check_conf(struct ps *ps,  const char *file_name, gchar *project_path)
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
	file_info = g_file_query_info(file, G_FILE_ATTRIBUTE_ACCESS_CAN_READ, 0, NULL, NULL);
	bret = g_file_info_get_attribute_boolean(file_info, G_FILE_ATTRIBUTE_ACCESS_CAN_READ);
	if (!bret) {
		pr_warn(ps, "Configuraion file not readable: %s", project_path_name);
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
	project = load_new_project(ps, keyfile, project_path);
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
