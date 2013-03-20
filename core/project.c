#include <string.h>
#include <assert.h>

#include "project.h"



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
	if (e->project_db_path) g_free(e->project_db_path);
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
	pr_info(ps, "deactiveate project");
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
	pr_info(ps, " project path:      %s", p->project_db_path);
}
