#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "perf-studio.h"
#include "log.h"


static gboolean is_valid_exec_file(const char *path)
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
	char *token, *tmp_path;
	gchar *tmp_str;

	assert(path);
	assert(cmd);

	tmp_path = (char *)path;

	if (cmd[0] == '/') {
		/* absolute path, ignore path argument */
		if (!is_valid_exec_file(cmd))
			return NULL;
		return g_strdup(cmd);
	}

	if (!strstr(tmp_path, ":")) {
		/* not a list of path, rater one path */
		tmp_str = g_strdup_printf("%s/%s", tmp_path, cmd);
		if (!is_valid_exec_file(cmd)) {
			log_print(LOG_ERROR, "not a executable: %s", tmp_str);
			g_free(tmp_str);
			return NULL;
		}

		return tmp_str;
	}

	token = strtok(tmp_path, ":");
	tmp_str = g_strdup_printf("%s/%s", token, cmd);
	if (is_valid_exec_file(cmd)) {
		return tmp_str;
	}
	g_free(tmp_str);

	while ((token = strtok(NULL, ":")) != NULL) {
		tmp_str = g_strdup_printf("%s/%s", token, cmd);
		if (is_valid_exec_file(cmd)) {
			return tmp_str;
		}
		g_free(tmp_str);
	}

	return NULL;
}
