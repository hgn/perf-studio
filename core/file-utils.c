#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "perf-studio.h"
#include "file-utils.h"
#include "log.h"


static gboolean valid_exec_file(const char *path)
{
	int ret;
	struct stat st;

	assert(path);

	ret = stat(path, &st);
	if (ret) {
		log_print(LOG_DEBUG, "File is not statable: %s", path);
		return FALSE;
	}

	if (st.st_mode & S_IEXEC)
		return TRUE;

	return FALSE;
}


/* returning string must be freed with gfree() */
gchar *file_utils_find_exec(const char *path, const char *cmd)
{
	char *token, *tmp_path, *cwd;
	gchar *tmp_str;

	assert(cmd);


	if (cmd[0] == '/') {
		/* absolute path, ignore path argument */
		if (cmd[1] == '\0') {
			return NULL;
		}
		if (!valid_exec_file(cmd))
			return NULL;
		return g_strdup(cmd);
	}

	if (cmd[0] == '.' && cmd[1] == '/') {
		/* relative path, ignore path argument */
		cwd = getcwd(NULL, 0);
		tmp_str = g_strdup_printf("%s/%s", cwd, &cmd[2]);
		free(cwd);
		if (!valid_exec_file(tmp_str)) {
			log_print(LOG_ERROR, "not a executable: %s", tmp_str);
			g_free(tmp_str);
			return NULL;
		}
		return tmp_str;
	}


	if (!path) {
		/* not absolute path and no PATHs, giving up */
		return NULL;
	}
	tmp_path = (char *)path;


	if (!strstr(tmp_path, ":")) {
		/* not a list of path, rater one path */
		tmp_str = g_strdup_printf("%s/%s", tmp_path, cmd);
		if (!valid_exec_file(tmp_str)) {
			log_print(LOG_ERROR, "not a executable: %s", tmp_str);
			g_free(tmp_str);
			return NULL;
		}

		return tmp_str;
	}

	token = strtok(tmp_path, ":");
	tmp_str = g_strdup_printf("%s/%s", token, cmd);
	if (valid_exec_file(tmp_str)) {
		return tmp_str;
	}
	g_free(tmp_str);

	while ((token = strtok(NULL, ":")) != NULL) {
		tmp_str = g_strdup_printf("%s/%s", token, cmd);
		if (valid_exec_file(tmp_str)) {
			return tmp_str;
		}
		g_free(tmp_str);
	}

	return NULL;
}
