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


static void add_events(struct ps *ps, struct module *module)
{
	struct event *e;

	(void) ps;

	e = event_new();
	module_add_event(module, e);
}


int register_module(struct ps *ps, struct module **module)
{
	struct module *m;

	m = module_new();
	if (!m)
		return -ENOBUFS;

	module_set_name(m, MODULE_NAME);
	module_set_description(m, MODULE_DESCRIPTION);

	add_events(ps, m);

	m->unregister_module = unregister_module;

	*module = m;

	return 0;
}

