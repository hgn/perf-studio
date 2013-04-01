#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "event.h"
#include "module-utils.h"
#include "shared.h"

const char *module_group_str(int id)
{
	/* if failed add missing case branch here */
	STATIC_ASSERT(MODULE_GROUP_MAX == 3);

	switch (id) {
	case MODULE_GROUP_CORE_ANALYSIS:
		return "Core Analysis";
		break;
	case MODULE_GROUP_APPLICATION_LEVEL:
		return "Application Level Analysis";
		break;
	case MODULE_GROUP_ARCHITECTURE_LEVEL:
		return "Architecture Level Analysis";
		break;
	default:
		return "UNKNOWN MODULE GROUP";
		break;
	};

	return "UNKNOWN MODULE GROUP";
}


struct module *module_new(void)
{
	return g_malloc0(sizeof(struct module));
}


void module_free(struct module *m)
{
	assert(m);
	g_free(m);
}


void module_set_name(struct module *m, const char *name)
{
	int len;

	if (!m || !name)
		return;

	len = min(MODULE_NAME_MAX - 1, (int)strlen(name) + 1);
	memcpy(m->name, name, len);
	m->name[len - 1] = '\0';
}

char *module_get_name(struct module *m)
{
	return m->name;
}


void module_set_description(struct module *m, const char *desc)
{
	int len;

	if (!m || !desc)
		return;

	len = min(MODULE_DESC_MAX - 1, (int)strlen(desc) + 1);
	memcpy(m->description, desc, len);
	m->description[len - 1] = '\0';
}


void module_set_group(struct module *m, int group)
{
	m->group = group;
}


int module_get_group(struct module *m)
{
	return m->group;
}


char *module_get_description(struct module *m)
{
	return m->description;
}


int module_add_events(struct module *m, struct events *e)
{
	if (!m || !e)
		return -EINVAL;

	m->events = e;

	return 0;
}


/* module_request_event_data() is called by a module
 * that data is requested. Normally a module is
 * signaled by perf-studio if new data is available
 * to display. But this can also be triggered by
 * the module. If new data is available perf-studio
 * will call module->update() to inform the module.
 */
void module_request_event_data(struct module *module)
{
	pr_info(module->ps, "Module %s request data", module->name);
}


