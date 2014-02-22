#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "module-utils.h"
#include "log.h"
#include "mc.h"


#define MODULE_NAME "Hello World"
#define MODULE_DESCRIPTION "print hello world"

struct hello_world_priv {
	struct mc_store *mc_store;
	GtkWidget *root;
};


static struct hello_world_priv *hello_world_priv_new(void)
{
	return g_malloc0(sizeof(struct hello_world_priv));
}


static void hello_world_priv_free(struct hello_world_priv *data)
{
	assert(data);
	g_free(data);
	data = NULL;
}


static void gui_msg_dialog(struct ps *ps, const gchar *format, ...)
{
	va_list  args;
	gchar *str;
	GtkWidget *dialog;
	GtkWidget *label, *content_area;

	va_start(args, format);
	str = g_strdup_vprintf (format, args);
	va_end(args);

	dialog = gtk_dialog_new_with_buttons("Message", GTK_WINDOW(ps->s.main_window),
					     GTK_DIALOG_DESTROY_WITH_PARENT, "OK",
					     GTK_RESPONSE_NONE, NULL);
	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	label = gtk_label_new(str);

	g_signal_connect_swapped(dialog, "response",
				 G_CALLBACK(gtk_widget_destroy), dialog);

	gtk_container_add(GTK_CONTAINER(content_area), label);
	gtk_widget_show_all(dialog);

	g_free(str);
}


static void unregister_module(struct ps *ps, struct module *module)
{
	struct hello_world_priv *data;

	(void)ps;

	data = module->data;
	assert(data);
	hello_world_priv_free(data);

	/* this will free module memory as well
	 * as child elements like events */
	module_free(module);
}


static struct mc_perf_record_data *hello_mc_create_perf_record_data(void)
{
	struct mc_perf_record_data *mc_perf_record_data;

	mc_perf_record_data = mc_perf_record_data_create();
	if (!mc_perf_record_data)
		return NULL;

	mc_perf_record_data_add_raw(mc_perf_record_data, "instructions");

	return mc_perf_record_data;
}

static void hello_mc_store_create(struct module *module,
			          struct hello_world_priv *hwp)
{
	int ret;
	struct mc_store *mc_store;
	struct mc_perf_record_data *mc_perf_record_data;

	assert(module);
	assert(hwp);

	mc_store = mc_store_alloc();
	mc_store_set_owner(mc_store, module);

	mc_perf_record_data = hello_mc_create_perf_record_data();
	ret = mc_store_add(mc_store, MEASUREMENT_CLASS_PERF_RECORD, mc_perf_record_data);
	if (ret != 0) {
		/* FIXME: proper error handling required, here
		 * return error code, free memory and check alone
		 * the part that the module cannot be used. In other
		 * words a clean error handling
		 */
		log_print(LOG_ERROR, "Cannot register perf record data");
		return;
	}

	/* remember a pointer to our mc_store. This
	 * is used later when module callbacks are
	 * called via module->data->mc_store */
	hwp->mc_store = mc_store;
}

void hello_mc_store_free_recursive(struct mc_store *mc_store)
{
	assert(mc_store);

	mc_store_free_recursive(mc_store);
}


static gboolean exec_analysis(gpointer data)
{
	struct ps *ps;
	struct module *module;

	assert(data);
	module = data;
	ps = module->ps;
	assert(ps);

	gui_msg_dialog(ps, "Exec not implemented yet");

	return TRUE;
}


static int activate_cb(struct module *module, GtkWidget **root)
{
	GtkWidget *vbox;
	GtkWidget *control_hbox;
	GtkWidget *exec_button;
	struct hello_world_priv *priv_data;

	assert(module);
	assert(module->ps);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	control_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	exec_button = gtk_button_new_with_label("Start Analyse Module");
	g_signal_connect_swapped(G_OBJECT(exec_button), "clicked",
				 G_CALLBACK(exec_analysis), module);
	gtk_box_pack_end(GTK_BOX(control_hbox), exec_button, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), control_hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(vbox);

	*root = vbox;
	priv_data = (struct hello_world_priv *)module->data;
	assert(priv_data);
	priv_data->root = vbox;

	return 0;
}


static int deactivate_cb(struct module *module)
{
	(void)module;

	return 0;
}


/*
 * this function is called after a disable() is called -
 * not in the beginning
 */
static int enable_cb(struct module *module)
{
	struct hello_world_priv *priv_data;

	assert(module);
	assert(module->data);

	log_print(LOG_INFO, "module %s enabled", module_get_name(module));

	priv_data = (struct hello_world_priv *) module->data;

	gtk_widget_set_sensitive(priv_data->root, TRUE);

	return 0;
}


static int disable_cb(struct module *module)
{
	struct hello_world_priv *priv_data;

	assert(module);
	assert(module->data);

	log_print(LOG_INFO, "module \"%s\" disabled", module_get_name(module));

	priv_data = (struct hello_world_priv *) module->data;

	gtk_widget_set_sensitive(priv_data->root, FALSE);

	return 0;
}


static int update_cb(struct module *module, enum update_type update_type, ...)
{
	(void)module;
	(void)update_type;

	return 0;
}


static void project_activated_cb(struct module *module, struct project *project)
{
	assert(module);
	assert(project);

	log_print(LOG_INFO, "module %s callback project_activated called",
		  module_get_name(module));
}


static void project_unloading_cb(struct module *module, struct project *project)
{
	assert(module);
	assert(project);

	log_print(LOG_INFO, "module %s callback project_unloading called",
		  module_get_name(module));
}


int register_module(struct ps *ps, struct module **module)
{
	struct hello_world_priv *hello_world_priv;
	struct module *m;

	(void)ps;

	m = module_new();
	if (!m)
		return -ENOBUFS;

	/* basic module setup */
	module_set_name(m, MODULE_NAME);
	module_set_description(m, MODULE_DESCRIPTION);
	module_set_group(m, MODULE_GROUP_CORE_ANALYSIS);
	module_set_maturity(m, MODULE_MATURITY_EXPERIMENTAL);

	/* register callbacks */
	m->update            = update_cb;

	/* module should do not draw anything (show standard screen)
	 * and should do no calculation: as less cpu cycles are used
	 * the better */
	m->activate          = activate_cb;
	m->deactivate        = deactivate_cb;

	/* unregister is the counterpart to this (register)
	 * module. After this function _all_ ressources must
	 * be freed */
	m->unregister_module = unregister_module;

	/* called whenn a project is activated or deactivated */
	m->project_activated  = project_activated_cb;
	m->project_unloading  = project_unloading_cb;

	m->enable  = enable_cb;
	m->disable = disable_cb;

	/* module private data */
	hello_world_priv = hello_world_priv_new();

	/* we also generate mc_store, the data structure describing
	 * all of our perf/traced data */
	hello_mc_store_create(m, hello_world_priv);

	/* we register our private data at the module user data pointer */
	m->data = hello_world_priv;

	*module = m;

	return 0;
}

