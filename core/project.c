#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <glib.h>
#include <glib/gstdio.h>

#include "project.h"


static void call_registered_activate_cb(struct ps *ps)
{
	GSList *tmp;

	tmp = ps->project_activate_cb_list;
	while (tmp) {
		void (*activate_callback)(struct ps *ps);

		activate_callback = tmp->data;
		pr_debug(ps, "Call project activate callback");
		activate_callback(ps);

		tmp = g_slist_next(tmp);
	}
}


/*
 * Called after a project is activated and ps->project
 * is a valid project pointer. Currently there is no unregister
 * function just because there is no need currently to support
 * this. Most important: modules are triggered by the core if
 * new projects are activated. The core is the active component,
 * not the module. So there core will register here and call
 * activated modules afterwards.
 *
 */
void project_register_activate_cb(struct ps *ps, void (*cb)(struct ps *ps))
{
	assert(ps);
	assert(cb);

	ps->project_activate_cb_list = g_slist_append(ps->project_activate_cb_list, cb);
}


/*
 * Callbacks shortly called before a project is deactivated. This
 * callback can be used to disable drawing, unset GtkLabels and
 * so on
 */
void project_register_deactivate_cb(struct ps *ps,  void (*cb)(struct ps *ps))
{
	assert(ps);
	assert(cb);

	ps->project_deactivate_cb_list = g_slist_append(ps->project_deactivate_cb_list, cb);
}


static void call_registered_deactivate_cb(struct ps *ps)
{
	GSList *tmp;

	tmp = ps->project_deactivate_cb_list;
	while (tmp) {
		void (*deactivate_callback)(struct ps *ps);

		deactivate_callback = tmp->data;
		pr_debug(ps, "Call project deactivate callback");
		deactivate_callback(ps);

		tmp = g_slist_next(tmp);
	}
}

/* return 0: success, <0 error */
static int check_create_refs_path(struct ps *ps, struct project *project)
{
	int ret = 0;

	assert(ps);
	assert(project);
	assert(project->project_path);
	assert(project->checksum);

	project->project_refs_path = g_build_filename(project->project_path, "refs", NULL);

	if (g_file_test(project->project_refs_path, G_FILE_TEST_IS_DIR)) {
		pr_info(ps, "%s already created", project->project_refs_path);
		ret = 0; /* no failure */
		goto out;
	}

	pr_info(ps, "Create refs directory %s", project->project_refs_path);
	ret = g_mkdir(project->project_refs_path, 0755);
	if (ret != 0) {
		pr_error(ps, "Failed to create refs directory %s", project->project_refs_path);
		ret = -EINVAL;
		goto out;
	}

	ret = 0;
out:
	return ret;
}


/* return 0: success, <0 error */
static int check_create_db_path(struct ps *ps, struct project *project)
{
	int ret = 0;
	GDateTime *date_time;
	gchar *refs_path, *date_time_fmt;

	assert(ps);
	assert(project);
	assert(project->project_path);
	assert(project->checksum);
	assert(project->project_refs_path);

	project->project_db_path = g_build_filename(project->project_path, "db", project->checksum, NULL);

	if (g_file_test(project->project_db_path, G_FILE_TEST_IS_DIR)) {
		pr_info(ps, "%s already created", project->project_db_path);
		ret = 0; /* no failure */
		goto out;
	}

	pr_info(ps, "Create project db path %s (0755)", project->project_db_path);
	ret = g_mkdir(project->project_db_path, 0755);
	if (ret != 0) {
		pr_error(ps, "Failed to create project db dir %s", project->project_db_path);
		ret = -EINVAL;
		goto out;
	}

	/* now symblink to new DB dir */
	date_time = g_date_time_new_now_local();
	date_time_fmt = g_date_time_format(date_time, "%Y-%m-%d-%H:%M:%S");
	refs_path = g_build_filename(project->project_refs_path, date_time_fmt, NULL);

	ret = symlink(project->project_db_path, refs_path);
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
	if (e->project_refs_path) g_free(e->project_refs_path);
	if (e->project_db_path) g_free(e->project_db_path);
	if (e->checksum) g_free(e->checksum);
	if (e->cmd_args_full) g_free(e->cmd_args_full);
	if (e->cmd_args_splitted) g_strfreev(e->cmd_args_splitted);

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

	call_registered_deactivate_cb(ps);

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

	call_registered_activate_cb(ps);
}


void project_show(struct ps *ps, struct project *p)
{
	gchar **tmp;
	int i = 1;

	pr_info(ps, " id:          %s", p->id);
	pr_info(ps, " cmd:         %s", p->cmd);
	pr_info(ps, " description: %s", p->description);
	pr_info(ps, " cmd-args:    %s", p->cmd_args_full);
	tmp = p->cmd_args_splitted;
	while (tmp && *tmp) {
		pr_info(ps, " cmd-arg %d:        %s", i, *tmp);
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

