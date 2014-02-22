#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "executer.h"
#include "module-utils.h"
#include "shared.h"
#include "gui-event-generator.h"
#include "log.h"


/*
 * This function is called when a module hit the "start analyze"
 * button. We first check if a project is loaded, if not we simple
 * return. If a project is loaded we now enforce a new execution.
 * In future releases we may check if the data is up to date. If so
 * we can show a "should we really do a new analyze" button. This
 * may be overwritten by configuration.
 */
void execute_module_triggered_analyze(struct module *module)
{
	struct ps *ps;

	assert(module);
	assert(module->ps);

	ps = module->ps;

	if (!ps->active_project) {
		log_print(LOG_INFO, "No project loaded - cannot do analyzes for project none");
		return;
	}

	log_print(LOG_INFO, "Now do analyzed for project!");
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
