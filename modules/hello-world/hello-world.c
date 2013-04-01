#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "module-utils.h"
#include "event.h"


#define MODULE_NAME "fooo"
#define MODULE_DESCRIPTION "print foo"

struct hello_world_priv {
	GSList *data_list;
};


static struct hello_world_priv *hello_world_priv_new(void)
{
	return g_malloc0(sizeof(struct hello_world_priv));
}


static void hello_world_priv_free(struct hello_world_priv *data)
{
	assert(data);

	g_slist_free(data->data_list);
	g_free(data);
}


static void unregister_module(struct ps *ps, struct module *module)
{
	struct events *e;
	struct hello_world_priv *data;

	(void)ps;

	e = module_get_events(module);
	assert(e);
	events_purge_all(e);

	data = module->data;
	assert(data);
	hello_world_priv_free(data);

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


static struct events *events_hello_world_new(void)
{
	struct events *e;

	e = events_new();
	add_counting_events(e);

	return e;
}


static int activate_cb(struct module *module, GtkWidget **root)
{
	GtkWidget *frame;

	assert(module);
	assert(module->ps);

	frame = gtk_frame_new("No data available or project open");
	gtk_container_set_border_width (GTK_CONTAINER (frame), 2);
	gtk_widget_set_size_request (frame, 100, 75);
	gtk_widget_show(frame);

	*root = frame;

	return 0;
}


static int deactivate_cb(struct module *module)
{
	(void)module;

	return 0;
}


static int update_cb(struct module *module, enum update_type update_type, ...)
{
	(void)module;
	(void)update_type;

	return 0;
}


int register_module(struct ps *ps, struct module **module)
{
	struct module *m;

	(void)ps;

	m = module_new();
	if (!m)
		return -ENOBUFS;

	/* basic module setup */
	module_set_name(m, MODULE_NAME);
	module_set_description(m, MODULE_DESCRIPTION);
	module_set_group(m, MODULE_GROUP_CORE_ANALYSIS);

	/* register module events */
	module_add_events(m, events_hello_world_new());

	/* register callbacks */
	m->update            = update_cb;
	m->activate          = activate_cb;
	m->deactivate        = deactivate_cb;
	m->unregister_module = unregister_module;

	/* module private data */
	m->data = hello_world_priv_new();

	*module = m;

	return 0;
}

