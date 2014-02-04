#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>


#include "perf-studio.h"

#include "shared.h"
#include "module-loader.h"
#include "module-utils.h"
#include "gui-toolkit.h"
#include "event.h"
#include "executer.h"

static void clean_module_data(struct module *module)
{
	free(module->path_name);
}


void unregister_all_modules(struct ps *ps)
{
	GSList *tmp;
	struct module *module;

	tmp = ps->module_list;
	while (tmp) {
		module = tmp->data;
		pr_info(ps, "deregister module \"%s\"", module->name);
		clean_module_data(module);
		module->unregister_module(ps, module);
		/* module is gone now ... */
		tmp = g_slist_next(tmp);
	}

	g_slist_free(ps->module_list);
}


static void register_module_global(struct ps *ps, struct module *module)
{
	ps->module_list = g_slist_append(ps->module_list, module);
}


/* return true if module is
 * already loaded, false otherwise */
static int module_path_dup_check(struct ps *ps, const char *path)
{
	struct module *m;
	GSList *tmp;

	tmp = ps->module_list;
	while (tmp) {
		m = tmp->data;
		assert(m);

		if (streq(path, m->path_name)) {
			pr_info(ps, "path dup: %s - %s",
				path,  m->path_name);
			return 1;
		}
		tmp = g_slist_next(tmp);
	}

	return 0;
}


/* return false for failure, true for success */
static int check_required_mod_callbacks(struct ps *ps, struct module *module)
{
	if (!module->update) {
		pr_error(ps, "Module update() callback not registered");
		return 0;
	}

	if (!module->activate) {
		pr_error(ps, "Module activate() callback not registered");
		return 0;
	}

	if (!module->deactivate) {
		pr_error(ps, "Module deactivate() callback not registered");
		return 0;
	}

	if (!module->unregister_module) {
		pr_error(ps, "Module unregister_module() callback not registered");
		return 0;
	}

	return 1;
}


static void register_module(struct ps *ps, const char *path, const char *name)
{
	int ret;
	char *path_name;
	void *handle;
	module_register_fn_t func;
	struct module *module;

	path_name = g_malloc(strlen(path) + strlen(name) + 2);

	strcpy(path_name, path);
	strcat(path_name, "/");
	strcat(path_name, name);

	ret = module_path_dup_check(ps, path_name);
	if (ret) {
		pr_error(ps, "Failed to load module %s",
			 path_name);
		goto err;
	}

	handle = dlopen(path_name, RTLD_NOW | RTLD_GLOBAL);
	if (!handle) {
		pr_warn(ps, "could not load plugin %s: %s\n", path_name, dlerror());
		goto err;
	}

	func = dlsym(handle, PERF_STUDIO_MODULE_REGISTER_FUNC);
	if (!func) {
		pr_warn(ps, "could not find func '%s' in plugin '%s'\n%s\n",
			PERF_STUDIO_MODULE_REGISTER_FUNC, path_name, dlerror());
		goto err;
	}

	pr_info(ps, "try to register module: %s", path_name);

	ret = func(ps, &module);
	if (ret != 0) {
		pr_warn(ps, "failed to register module %s", path_name);
		dlclose(handle);
		goto err;
	}


	pr_info(ps, " module name:        \"%s\"", module_get_name(module));
	pr_info(ps, " module description: \"%s\"", module_get_description(module));
	if (MSG_LEVEL_INFO(ps)) {
		pr_info(ps, " registred events:");
	}

	ret = check_required_mod_callbacks(ps, module);
	if (!ret) {
		pr_error(ps, "Module %s (%s) not registered",
			 module_get_name(module), path_name);
		goto err;
	}

	module->dl_handle = handle;
	module->path_name = path_name;
	module->ps        = ps;

	register_module_global(ps, module);

	return;
err:
	g_free(path_name);
}


/*
 * Returns negative error code in the case
 * of a critical error - not when no module was
 * found in the path
 */
static int register_modules_dir(struct ps *ps, const char *path)
{
	int ret;
	struct dirent *dent;
	struct stat xstat;
	DIR *dir;
	const char *suffix = ".so";

	ret = stat(path, &xstat);
	if (ret < 0)
		goto out;

	if (!S_ISDIR(xstat.st_mode))
		goto out;

	dir = opendir(path);
	if (!dir)
		goto out;

	while ((dent = readdir(dir))) {
		const char *name = dent->d_name;

		if (strcmp(name, ".") == 0 ||
		    strcmp(name, "..") == 0)
			continue;

		/* Only load plugins that end in suffix */
		if (strlen(name) <= strlen(suffix))
			continue;

		if (!streq(name + (strlen(name) - strlen(suffix)), suffix))
			continue;

		register_module(ps, path, name);
	}

	closedir(dir);

out:
	return 0;
}

/*
 * Returns negative error code in the case
 * of a critical error - not when no module was
 * found in the path
 */
int register_available_modules(struct ps *ps)
{
	int ret;
        char *envdir;
	gchar *path, *cwd;
	gchar **conf_tmp;
	const gchar *home;

#ifdef MODULE_DIR
	pr_info(ps, "search modules in: %s", MODULE_DIR);
	ret = register_modules_dir(ps, MODULE_DIR);
	if (ret != 0) {
		pr_error(ps, "failed to register system plugins in %s", MODULE_DIR);
		return ret;
	}
#endif

	envdir = getenv("PERF_STUDIO_MODULE_DIR");
	if (envdir) {
		pr_info(ps, "search modules in: %s", envdir);
		ret = register_modules_dir(ps, MODULE_DIR);
		if (ret != 0) {
			pr_error(ps, "failed to register plugins in %s", envdir);
			return ret;
		}
	}

	/* ~/.config/perf-studio/conf */
	conf_tmp = ps->conf.common.module_paths;
	while (conf_tmp && *conf_tmp) {
		pr_info(ps, "search modules in: %s", *conf_tmp);
		ret = register_modules_dir(ps, *conf_tmp);
		if (ret != 0) {
			pr_error(ps, "failed to register modules in %s", *conf_tmp);
			return ret;
		}
		conf_tmp++;
	}

	home = g_getenv("HOME");
	if (home) {
		path = g_malloc(strlen(home) + strlen(LOCAL_MODULES_DIR) + 2);
		strcpy(path, home);
		strcat(path, "/");
		strcat(path, LOCAL_MODULES_DIR);
		pr_info(ps, "search modules in: %s", path);
		ret = register_modules_dir(ps, path);
		g_free(path);
		if (ret != 0) {
			pr_error(ps, "failed to register modules in %s", path);
			return ret;
		}
	}

	cwd = get_current_dir_name();
	if (cwd) {
		pr_info(ps, "search modules in: %s", cwd);
		ret = register_modules_dir(ps, cwd);
		if (ret != 0) {
			pr_error(ps, "failed to register plugins in %s", cwd);
			free(cwd);
			return ret;
		}
	}
	free(cwd);

	return 0;
}

static void module_deactivate(struct ps *ps, struct module *module)
{
	executer_unregister_module_events(ps, module);
	module->deactivate(module);
}


static gboolean close_module_tab_cb(gpointer data)
{
	struct ps *ps;
	struct module *module;

	assert(data);
	module = data;
	ps = module->ps;
	assert(ps);

	module_deactivate(ps, module);
	gtk_notebook_remove_page(GTK_NOTEBOOK(ps->s.amc_notebook), module->notebook_id);
	module->activated = 0;

	return TRUE;
}


static GtkWidget *close_tab_button(struct ps *ps, struct module *module)
{
	GtkWidget *hbox;
	GtkWidget *label;
	GtkWidget *button;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	label = gtk_label_new(module->name);
	gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 0);

	button = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE );
	gtk_button_set_alignment(GTK_BUTTON(button ), 0, 1 );
	gtk_button_set_image(GTK_BUTTON(button), ps_icon(ps, ICON_CLOSE));
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
				 G_CALLBACK(close_module_tab_cb), module);

	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);

	gtk_widget_show_all(hbox);

	return hbox;
}


static GtkWidget *create_module_top_status_bar(struct ps *ps, struct module *module)
{
	GtkWidget *hbox;
	GtkWidget *control_hbox;
	GtkWidget *button;

	(void) ps;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	button = gtk_button_new_with_label("Close Module");
	gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	g_signal_connect_swapped(G_OBJECT(button), "clicked",
				 G_CALLBACK(close_module_tab_cb), module);

	gtk_widget_show_all(hbox);

	return hbox;
}


static GtkWidget *create_module_container(struct ps *ps)
{
	GtkWidget *vbox;

	(void) ps;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	gtk_widget_show_all(vbox);

	return vbox;
}


static void module_activate(struct ps *ps, struct module *module)
{
        GtkWidget *label, *h, *v;
	GtkWidget *scroll_widget;
	GtkWidget *module_widget = NULL;

	if (module->activated) {
		pr_info(ps, "Module %s already activated!", module->name);
		return;
	}

	pr_debug(ps, "Activate module %s", module->name);
	module->activate(module, &module_widget);

	if (!module_widget) {
		pr_error(ps, "Module %s did not create a GtkWidget",
			 module->name);
		return;
	}

	scroll_widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_widget),
					    GTK_SHADOW_OUT);

	v = create_module_container(ps);
	h = create_module_top_status_bar(ps, module);

	gtk_box_pack_start(GTK_BOX(v), h,  FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(v), module_widget,  TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(scroll_widget), v);
	gtk_widget_show_all(scroll_widget);

	/* add tab panel to netebook */
	label = close_tab_button(ps, module);
	module->notebook_id = gtk_notebook_append_page(GTK_NOTEBOOK(ps->s.amc_notebook),
						       scroll_widget, label);
	if (module->notebook_id < 0) {
		pr_error(ps, "Failed to add page to notebook");
		return;
	}

	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(ps->s.amc_notebook), scroll_widget, TRUE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(ps->s.amc_notebook), 1);

	pr_debug(ps, "Module \"%s\" activated", module->name);

	module->activated = 1;
}


void module_activated_by_name(struct ps *ps, const char *module_name)
{
	GSList *tmp;
	struct module *module;

	tmp = ps->module_list;
	while (tmp) {
		module = tmp->data;
		if (streq(module->name, module_name)) {
			pr_info(ps, "activate module by name\"%s\"", module->name);
			module_activate(ps, module);
			return;
		}
		tmp = g_slist_next(tmp);
	}
}
