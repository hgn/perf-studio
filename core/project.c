#include <string.h>

#include "project.h"



struct project *project_new(void)
{
	return g_malloc0(sizeof(struct project));
}


void project_free(struct project *e)
{
	if (e->exec_path) g_free(e->exec_path);
	if (e->project_path) g_free(e->project_path);
	if (e->exec_args) g_strfreev(e->exec_args);
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


void project_show(struct ps *ps, struct project *p)
{
	gchar **tmp;
	int i = 1;

	pr_info(ps, " project path:      %s", p->project_path);
	pr_info(ps, " exec-path:         %s", p->exec_path);
	tmp = p->exec_args;
	while (tmp && *tmp) {
		pr_info(ps, " exec-arg %d:        %s", i, *tmp);
		tmp++; i++;
	}
	pr_info(ps, " working directory:   ");
	pr_info(ps, " environment:         ");
	pr_info(ps, " nice level:          ");
	pr_info(ps, " scheduler policy:    ");
	pr_info(ps, " IO scheduler:        ");

}
