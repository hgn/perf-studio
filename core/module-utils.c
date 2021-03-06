#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "module-utils.h"
#include "shared.h"
#include "executer.h"

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
}


struct module *module_new(void)
{
	struct module *m;

	m = g_malloc0(sizeof(struct module));

	/*
	 * per default all modules are considered
	 * as stable, new module must mark them
	 * as experimantal is wished otherwise
	 */
	m->maturity = MODULE_MATURITY_STABLE;

	return m;
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


const char *module_maturity_str(struct module *module)
{
	switch (module->maturity) {
	case MODULE_MATURITY_STABLE:
		return "stable";
	case MODULE_MATURITY_EXPERIMENTAL:
		return "experimental";
	default:
		assert(0);
		return "unknown";
	};
}


void module_set_maturity(struct module *m, int maturity)
{
	m->maturity = maturity;
}


void module_new_data_available(struct module *m, struct mc_store *mc_store)
{
	assert(m);
	assert(mc_store);
	assert(m->update);

	m->update(m, mc_store);
}
