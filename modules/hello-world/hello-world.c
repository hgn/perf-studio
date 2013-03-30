#include <errno.h>

#include "perf-studio.h"
#include "module-utils.h"
#include "event.h"


#define MODULE_NAME "fooo"
#define MODULE_DESCRIPTION "print foo"

static void unregister_module(struct ps *ps, struct module *module)
{
	(void) ps;

	/* this will free module memory as well
	 * as child elements like events */
	module_free(module);
}


static void add_counting_events(struct events *events)
{
	struct event *e;

	e = event_new();
	e->type = EVENT_TYPE_COUNTING;
	e->counting.event_code = EVENT_PERF_CYCLES;
	e->counting.where      = USERKERNELSPACE;

	events_add_event(events, e);
}


static void add_events(struct ps *ps, struct module *module)
{
	struct events *e;

	(void) ps;

	e = events_new();

	add_counting_events(e);

	module_add_events(module, e);
}


int register_module(struct ps *ps, struct module **module)
{
	struct module *m;

	m = module_new();
	if (!m)
		return -ENOBUFS;

	module_set_name(m, MODULE_NAME);
	module_set_description(m, MODULE_DESCRIPTION);
	module_set_group(m, MODULE_GROUP_COMMON);

	add_events(ps, m);

	m->unregister_module = unregister_module;

	*module = m;

	return 0;
}

