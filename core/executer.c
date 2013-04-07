#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "executer.h"
#include "module-utils.h"
#include "shared.h"
#include "event.h"
#include "event-generator.h"
#include "shared.h"
#include "gui-event-generator.h"


struct project_events_data {
	struct events *events;
	struct module *module;
};


static struct project_events_data *project_events_data_new(void)
{
	return g_malloc0(sizeof(struct project_events_data));
}


static void project_events_data_free(struct project_events_data *data)
{
	g_free(data);
}



static int register_events_at_project(struct ps *ps, struct project *project,
				      struct module *module)
{
	GSList *tmp;
	struct project_events_data *data;

	/* check if the module is alreay registered
	 * at the project, this should not happend */
	tmp = project->events_list;
	while (tmp) {
		data = tmp->data;
		assert(data);

		if (data->module == module) {
			pr_error(ps, "Already registered, not a problem, but should not happend");
			return 0;
		}
		//g_slist_remove_link
		tmp = g_slist_next(tmp);
	}

	data = project_events_data_new();
	data->events = module->events;
	data->module = module;

	project->events_list = g_slist_append(project->events_list, data);

	return 0;
}


void executer_register_module_events(struct ps *ps, struct module *module)
{
	int ret;

	assert(ps);
	assert(module);
	assert(module->events);

	if (!ps->project) {
		pr_info(ps, "No project open, so we do not register here");
		return;
	}

	ret = register_events_at_project(ps, ps->project, module);
	if (ret < 0) {
		pr_error(ps, "Failed to register events for project");
		return;
	}



	gui_event_executer_setup(ps, ps->project);
}


void executer_unregister_module_events(struct ps *ps, struct module *module)
{
	GSList *tmp;
	struct project *project;

	assert(ps);
	assert(module);

	if (!ps->project) {
		pr_info(ps, "Unregister module - no project active");
		return;
	}

	/* We assume that project is active. If we extend
	 * perf-studio to load a new project or de-activate
	 * a project we must add an additional function here
	 * in executer to deregister all events for a project
	 *
	 * The previous assert should crash the program if in
	 * future this unload feature is implemented.
	 */
	project = ps->project;

	tmp = project->events_list;
	while (tmp) {
		struct project_events_data *data;
		data = tmp->data;
		assert(data);
		assert(data->module);

		if (data->module == module) {
			project->events_list = g_slist_remove_link(project->events_list, tmp);
			project_events_data_free(data);
			pr_info(ps, "remove module from observed events for project");
			return;
		}
		tmp = g_slist_next(tmp);
	}

	pr_error(ps, "Could not find events for project in project list, mmh");
}


#if 0
/* iterate over all registered modules,
 * check if the particular events are registerd
 * and if so then the module is informed via update()
 */
static events_multiplexer(struct *ps)
{
}

/* called when executer ends and data was produced
 * then this function is called which in turn calls events multiplexer
 */
static events_update()
#endif
