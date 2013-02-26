#define _GNU_SOURCE

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
#include "modules.h"
#include "module-utils.h"

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

	/* FIXME: GSlist must be destroyed too */
}


static void register_module_global(struct ps *ps, struct module *module)
{
	ps->module_list = g_slist_append(ps->module_list, module);
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

	pr_info(ps, "registering module: %s", path_name);

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
		module_print_registered_events(ps, module);
	}

	module->dl_handle = handle;
	module->path_name = path_name;

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
	gchar *path;
	const gchar *home;
	gchar *cwd;

#ifdef MODULE_DIR
	pr_info(ps, "search modules in: %s", MODULE_DIR);
	ret = register_modules_dir(ps, MODULE_DIR);
	if (ret != 0) {
		pr_error(ps, "failed to register system plugins in %s", MODULE_DIR);
		return ret;
	}
#endif

	envdir = getenv("TRACE_CMD_PLUGIN_DIR");
	if (envdir) {
		pr_info(ps, "search modules in: %s", envdir);
		ret = register_modules_dir(ps, MODULE_DIR);
		if (ret != 0) {
			pr_error(ps, "failed to register plugins in %s", envdir);
			return ret;
		}
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
			return ret;
		}
	}

	return 0;
}


