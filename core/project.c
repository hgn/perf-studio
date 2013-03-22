#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>


#include <glib.h>
#include <glib/gstdio.h>

#include "project.h"

/* return 0: success, <0 error */
static int check_create_refs_path(struct ps *ps, struct project *project)
{
	int ret = 0;
	gchar *path;

	assert(ps);
	assert(project);
	assert(project->project_path);
	assert(project->checksum);

	path = g_build_filename(project->project_path, "refs", NULL);

	if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
		pr_info(ps, "%s already created", path);
		ret = 0; /* no failure */
		goto out;
	}

	pr_info(ps, "Create refs directory %s", path);
	ret = g_mkdir(path, 0755);
	if (ret != 0) {
		pr_error(ps, "Failed to create refs directory %s", path);
		ret = -EINVAL;
		goto out;
	}

	ret = 0;
out:
	g_free(path);
	return ret;
}


/* return 0: success, <0 error */
static int check_create_db_path(struct ps *ps, struct project *project)
{
	int ret = 0;
	GDateTime *date_time;
	gchar *path, *refs_path, *date_time_fmt;

	assert(ps);
	assert(project);
	assert(project->project_path);
	assert(project->checksum);

	path = g_build_filename(project->project_path, "db", project->checksum, NULL);

	if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
		pr_info(ps, "%s already created", path);
		ret = 0; /* no failure */
		goto out;
	}

	pr_info(ps, "Create DB-VERSION path %s", path);
	ret = g_mkdir(path, 0755);
	if (ret != 0) {
		pr_error(ps, "Failed to create DB dir %s", path);
		ret = -EINVAL;
		goto out;
	}

	/* now symblink to new DB dir */
	date_time = g_date_time_new_now_local();
	date_time_fmt = g_date_time_format(date_time, "%Y-%m-%d-%H:%M:%S");
	refs_path = g_build_filename(project->project_path, "refs", date_time_fmt, NULL);

	ret = symlink(path, refs_path);
	if (ret != 0) {
		pr_error(ps, "Could not create symlink (%s) to db", refs_path);
		goto out2;
	}

	ret = 0;

out2:
	g_free(date_time_fmt);
	g_date_time_unref(date_time);
	g_free(refs_path);

out:
	g_free(path);
	return ret;
}


/* return 0: success, <0 error */
static int check_create_db_parent_dir(struct ps *ps, struct project *project)
{
	int ret = 0;
	gchar *path;

	assert(ps);
	assert(project);
	assert(project->project_path);
	assert(project->checksum);

	path = g_build_filename(project->project_path, "db", NULL);

	if (g_file_test(path, G_FILE_TEST_IS_DIR)) {
		pr_info(ps, "project db directory %s already created", path);
		ret = 0; /* no failure */
		goto out;
	}

	pr_info(ps, "Create db parent directory %s", path);
	ret = g_mkdir(path, 0755);
	if (ret != 0) {
		pr_error(ps, "Failed to create DB parent dir %s", path);
		ret = -EINVAL;
		goto out;
	}


	ret = 0;
out:
	g_free(path);
	return ret;
}


/* return 0: success, <0 error */
static int check_create_db_info(struct ps *ps, struct project *project)
{
	int ret = 0;
	gchar *path;

	assert(ps);
	assert(project);
	assert(project->project_path);
	assert(project->checksum);

	path = g_build_filename(project->project_path, "db", project->checksum, "info",  NULL);

	if (g_file_test(path, G_FILE_TEST_IS_REGULAR)) {
		pr_info(ps, "project conf %s already created", path);
		ret = 0; /* no failure */
		goto out;
	}

	pr_info(ps, "Create db conf file %s", path);
	ret = g_creat(path, 0644);
	if (ret < 0) {
		pr_error(ps, "Failed to create config in DB dir %s", path);
		ret = -EINVAL;
		goto out;
	}
	close(ret);

	ret = 0;
out:
	g_free(path);
	return ret;
}


struct project *project_new(void)
{
	return g_malloc0(sizeof(struct project));
}


void project_free(struct project *e)
{
	assert(e);
	assert(e->id);
	assert(e->cmd);

	/* required */
	g_free(e->id);
	g_free(e->cmd);

	/* optional */
	if (e->description) g_free(e->description);
	if (e->project_path) g_free(e->project_path);
	if (e->checksum) g_free(e->checksum);
	if (e->cmd_args) g_strfreev(e->cmd_args);

	g_free(e);
}

void project_purge_all(struct ps *ps)
{
	GSList *tmp;
	struct project *project;

	pr_info(ps, "deregister all loaded project");

	tmp = ps->project_list;
	while (tmp) {
		project = tmp->data;
		pr_info(ps, "deregister project");
		pr_info(ps, "  cmd: \"%s\"", project->cmd);
		project_free(project);

		tmp = g_slist_next(tmp);
	}

	g_slist_free(ps->project_list);
}


/* deactivate ps->project */
void project_deactivate(struct ps *ps)
{
	struct project *project;

        assert(ps);
        assert(ps->project);

	project = ps->project;

        pr_info(ps, "deactivate project %s", project->id);

        /*
         * we free/deactive all values which we previously
         * generated at active state
         */
        if (project->checksum) g_free(project->checksum);

        ps->project = NULL;
}

#define BUF_SIZE 4096

static gchar *build_checksum(GChecksumType type, const gchar *file_path)
{
        int fd;
        const gchar *check;
        gchar *checksum_string;
        ssize_t res;
        char buffer[BUF_SIZE];
        GChecksum *checksum;

        fd = open(file_path, O_RDONLY);
        if (fd == -1)
                return NULL;

        checksum = g_checksum_new(type);

        while ((res = read(fd, buffer, BUF_SIZE)) > 0)
                g_checksum_update(checksum, (const guchar *)buffer, (size_t)res);


        check = g_checksum_get_string(checksum);
        assert(check);
        checksum_string = g_strdup(check);

        g_checksum_free(checksum);
        close(fd);

        return checksum_string;
}


static gboolean is_absolute_path(const gchar *cmd)
{
        if (strlen(cmd) < 2)
                return FALSE;

        if (cmd[0] == '/')
                return TRUE;

        return FALSE;
}


/* called when ps->project becomes valid. We check
 * some values (sanity) and compute temp values */
void project_activate(struct ps *ps, struct project *project)
{
	int ret;

	assert(ps);
	assert(project);
	assert(ps->project == NULL);

	ps->project = project;
	project->status = PROJECT_STATUS_SOMEHOW_INVALID;

	pr_info(ps, "activate project %s", project->id);

	if (!is_absolute_path(project->cmd)) {
		pr_error(ps, "%s must be a absolute path", project->cmd);
		project->status = PROJECT_STATUS_CMD_PATH_INVALID;
		return;
	}

	if (!g_file_test(project->cmd, G_FILE_TEST_IS_EXECUTABLE)) {
		pr_error(ps, "%s is not executable", project->cmd);
		project->status = PROJECT_STATUS_CMD_NOT_EXECUTABLE;
		return;
	}

	project->checksum = build_checksum(G_CHECKSUM_MD5, project->cmd);
	assert(project->checksum);

	ret = check_create_db_parent_dir(ps, project);
	if (ret < 0) {
		project->status = PROJECT_STATUS_SOMEHOW_INVALID;
		return;
	}

	ret = check_create_refs_path(ps, project);
	if (ret < 0) {
		project->status = PROJECT_STATUS_SOMEHOW_INVALID;
		return;
	}


	ret = check_create_db_path(ps, project);
	if (ret < 0) {
		project->status = PROJECT_STATUS_SOMEHOW_INVALID;
		return;
	}

	ret = check_create_db_info(ps, project);
	if (ret < 0) {
		project->status = PROJECT_STATUS_SOMEHOW_INVALID;
		return;
	}

	project->status = PROJECT_STATUS_OK;
}


void project_show(struct ps *ps, struct project *p)
{
	gchar **tmp;
	int i = 1;

	pr_info(ps, " id:         %s", p->id);
	pr_info(ps, " cmd:         %s", p->cmd);
	pr_info(ps, " description: %s", p->description);
	tmp = p->cmd_args;
	while (tmp && *tmp) {
		pr_info(ps, " cmd-args %d:        %s", i, *tmp);
		tmp++; i++;
	}
	pr_info(ps, " working directory:   ");
	pr_info(ps, " environment:         ");
	pr_info(ps, " nice level:          ");
	pr_info(ps, " scheduler policy:    ");
	pr_info(ps, " IO scheduler:        ");
	pr_info(ps, " project path:      %s", p->project_path);
}


/*
 * returns 0 of the project could be loaded
 * returns negative error code otherwise. The
 * function iterate over project list
 */
int project_load_by_id(struct ps *ps, const char *id)
{
	GSList *list_tmp;

        if (ps->project)
                project_deactivate(ps);

	list_tmp = ps->project_list;
	while (list_tmp) {
		struct project *project;
		project = list_tmp->data;

		if (streq(project->id, id)) {
			project_activate(ps, project);
			return 0;
		}

		list_tmp = g_slist_next(list_tmp);
	}

	return -EINVAL;
}
