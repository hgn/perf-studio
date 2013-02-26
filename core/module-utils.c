#include <string.h>
#include <errno.h>

#include "perf-studio.h"
#include "event.h"
#include "module-utils.h"


struct module *module_new(void)
{
	return g_malloc0(sizeof(struct module));
}


void module_free(struct module *m)
{
	module_purge_all_events(m);
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

char *module_get_description(struct module *m)
{
	return m->description;
}


int module_add_event(struct module *m, struct event *e)
{
	if (!m || !e)
		return -EINVAL;

	m->event_list = g_slist_append(m->event_list, e);

	return 0;
}


void module_purge_all_events(struct module *m)
{
	GSList *tmp;

	tmp = m->event_list;
	while (tmp) {
		struct event *e = tmp->data;
		event_free(e);
		tmp = g_slist_next(tmp);
	}

	/* FIXME: event_list must be freed too */
}

void module_print_registered_events(struct ps *ps, struct module *module)
{
	GSList *tmp;

	tmp = module->event_list;
	while (tmp) {
		struct event *e = tmp->data;
		event_print(ps, e);
		tmp = g_slist_next(tmp);
	}
}

