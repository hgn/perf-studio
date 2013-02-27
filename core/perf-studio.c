

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <gtk/gtk.h>

#include "perf-studio.h"

#include "shared.h"
#include "modules.h"
#include "conf-file.h"
#include "project.h"
#include "version.h"
#include "random.h"


int parse_cli_options(struct ps *ps, int ac, char **av)
{
	int c;

	ps->args.me = g_strdup(av[0]);

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"verbose",          1, 0, 'v'},
			{"modules",          0, 0, 'm'},
			{0, 0, 0, 0}
		};

		c = getopt_long(ac, av, "vm", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 'v':
			ps->args.msg_level--;
			break;
		case 'm':
			ps->args.list_available_modules = TRUE;
			break;
		case '?':
			break;

		default:
			err_msg(ps, "getopt returned character code 0%o ?", c);
			return -EINVAL;
		}
	}

	return 0;
}


static struct ps *ps_new(void)
{
	struct ps *ps;

	ps = g_malloc0(sizeof(struct ps));

	ps->screen_usable = FALSE;

	ps->args.list_available_modules = FALSE;
	ps->args.msg_level = DEFAULT_MSG_LEVEL;

	ps->module_list = NULL;

	return ps;
}


static void ps_conf_free(struct conf *conf)
{
	if (conf->perf_exec_path) g_free(conf->perf_exec_path);
	if (conf->module_paths) g_strfreev(conf->module_paths);
}


static void ps_free(struct ps *ps)
{
	assert(ps);

	if (ps->args.me)
		g_free(ps->args.me);

	ps_conf_free(&ps->conf);

	free(ps);
}


int main (int ac, char **av)
{
	int ret;
	struct ps *ps;

	ps = ps_new();

	pr_info(ps, "Perf-Studio (C)");
	pr_info(ps, "Version: %s", VERSION_STRING);

	rand_init(ps);
	gchar *s = rand_hex_string(ps, 23);
	pr_info(ps, "foo: %s", s);
	g_free(s);

	ret = parse_cli_options(ps, ac, av);
	if (ret != 0) {
		err_msg(ps, "failed to parse command line");
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = load_user_conf_file(ps);
	if (ret != 0) {
		err_msg(ps, "failed to parse configuration file");
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = load_project_conf_file(ps);
	if (ret != 0) {
		err_msg(ps, "failed to load project configuration file");
		ret = EXIT_FAILURE;
		goto out;
	}


	ret = register_available_modules(ps);
	if (ret != 0) {
		err_msg(ps, "failed to register perf-studio modules");
		ret = EXIT_FAILURE;
		goto out;
	}

	gtk_init(&ac, &av);

	ps->s.main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	/* gtk_main_quit() simple leave the main loop */
	g_signal_connect(ps->s.main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show(ps->s.main_window);

	gtk_main();


	ret = EXIT_SUCCESS;
out:
	unregister_all_modules(ps);
	project_purge_all(ps);
	rand_free(ps);
	ps_free(ps);
	pr_info(ps, "exiting");
	return ret;
}
