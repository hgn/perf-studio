#include <string.h>

#include "project.h"



struct project *project_new(void)
{
	return g_malloc0(sizeof(struct project));
}


void project_free(struct project *e)
{
	if (e->exec_path)
		g_free(e->exec_path);
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
		pr_info(ps, "  exec path: \"%s\"", project->exec_path);
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
