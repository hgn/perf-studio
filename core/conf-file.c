#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "perf-studio.h"
#include "shared.h"
#include "project.h"

#define PERF_STUDIO_USER_CONF_DIR "perf-studio"
#define PERF_STUDIO_USER_GLOBAL_CONF_NAME "perf-studio.conf"
#define PERF_STUDIO_USER_PROJECT_CONF_NAME "projects.conf"


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


	/* Read in data from the key file from the group "username". */
	ps->conf.perf_exec_path = g_key_file_get_string(keyfile, "common", "perf-path", NULL);
	pr_info(ps, "perf-path: %s", ps->conf.perf_exec_path);

	ps->conf.module_paths = g_key_file_get_string_list(keyfile, "common", "module-paths", &length, NULL);
	iter = ps->conf.module_paths;
	while (iter && *iter) {
		pr_info(ps, "module-paths: %s", *iter);
		iter++;
	}

out:
	g_free(full_path);
	g_key_file_free(keyfile);
	return 0;
}

#define CONFIG_NAME "config"


/* foo/bar/project-foo/.perf-studio/config */
static void load_new_project(struct ps *ps, const char *path)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	gchar *full_path, *exec_path;
	struct project *project;

	full_path = g_malloc(strlen(path) +
			1 + /* "/" */
			strlen(CONFIG_NAME) +
			1); /* "\0" */

	strcpy(full_path, path);
	strcat(full_path, "/");
	strcat(full_path, CONFIG_NAME);

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file(keyfile, full_path, flags, NULL)) {
		pr_info(ps, "failed to open project configuration file [%s]", full_path);
		goto out;
	}
	/* Read in data from the key file from the group "username". */
	exec_path = g_key_file_get_string(keyfile, "common", "exec-path", NULL);
	pr_info(ps, "command exec-path: %s", exec_path);
	pr_info(ps, "command args: ");
	pr_info(ps, "command working directory: ");
	pr_info(ps, "command environment: ");
	pr_info(ps, "command nice level: ");
	pr_info(ps, "command scheduler policy: ");
	pr_info(ps, "command IO scheduler: ");

	/* ok, project seems sane, create it */
	project = project_new();
	project->exec_path = exec_path;

	pr_info(ps, "project seems fine - add to list");
	project_add(ps, project);

out:
	g_free(full_path);
	g_key_file_free(keyfile);
}


int load_project_conf_file(struct ps *ps)
{
	GKeyFile *keyfile;
	GKeyFileFlags flags;
	const gchar *data_dir;
	gchar *full_path;
	gchar **groups, **groups_tmp;

	data_dir = g_get_user_config_dir();
	full_path = g_malloc(strlen(data_dir) +
			1 + /* "/" */
			strlen(PERF_STUDIO_USER_CONF_DIR) +
			1 + /* "/" */
			strlen(PERF_STUDIO_USER_PROJECT_CONF_NAME) +
			1); /* "\0" */

	strcpy(full_path, data_dir);
	strcat(full_path, "/");
	strcat(full_path, PERF_STUDIO_USER_CONF_DIR);
	strcat(full_path, "/");
	strcat(full_path, PERF_STUDIO_USER_PROJECT_CONF_NAME);

	keyfile = g_key_file_new();
	flags = G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS;

	if (!g_key_file_load_from_file(keyfile, full_path, flags, NULL)) {
		pr_info(ps, "failed to open project configuration file [%s]", full_path);
		goto out;
	}

	groups = g_key_file_get_groups(keyfile, NULL);
	groups_tmp = groups;
	while (groups_tmp && *groups_tmp) {
		gchar *project_path = g_key_file_get_string(keyfile, *groups_tmp, "path", NULL);
		pr_info(ps, "project-path: %s", project_path);
		load_new_project(ps, project_path);
		g_free(project_path);
		groups_tmp++;
	}

out:
	g_free(full_path);
	g_strfreev(groups);
	g_key_file_free(keyfile);
	return 0;
}
