

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <gtk/gtk.h>

#include "perf-studio.h"
#include "shared.h"
#include "modules.h"
#include "conf-file.h"
#include "project.h"
#include "version.h"
#include "random.h"
#include "gui-main.h"


int parse_cli_options(struct ps *ps, int ac, char **av)
{
	int c;

	ps->args.me = g_strdup(av[0]);
	ps->args.theme = THEME_DARK;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"theme",            1, 0, 't'},
			{"verbose",          1, 0, 'v'},
			{"modules",          0, 0, 'm'},
			{0, 0, 0, 0}
		};

		c = getopt_long(ac, av, "vm", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 't':
			if (streq("dark", optarg)) {
				ps->args.theme = THEME_DARK;
			} else if (streq("light", optarg)) {
				ps->args.theme = THEME_LIGHT;
			}
			break;
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
	if (conf->common.perf_path) g_free(conf->common.perf_path);
	if (conf->common.username) g_free(conf->common.username);
	if (conf->common.module_paths) g_strfreev(conf->common.module_paths);
}


static void ps_free(struct ps *ps)
{
	assert(ps);

	if (ps->args.me)
		g_free(ps->args.me);
	if (ps->si.pixmapdir) g_free(ps->si.pixmapdir);
	if (ps->si.buttondir) g_free(ps->si.buttondir);

	ps_conf_free(&ps->conf);

	free(ps);
}



static void quit_perf_studio_sig(int sig)
{
	psignal(sig, "perf-studio");
	fprintf(stderr, "signaled enforced exit received\n");
	gtk_main_quit();
}


static void register_signal_handler(struct ps *ps)
{
	signal(SIGSEGV, quit_perf_studio_sig);
	signal(SIGFPE,  quit_perf_studio_sig);
	signal(SIGINT,  quit_perf_studio_sig);
	signal(SIGQUIT, quit_perf_studio_sig);
	signal(SIGTERM, quit_perf_studio_sig);
}


int main (int ac, char **av)
{
	int ret;
	struct ps *ps;

	ps = ps_new();
	g_type_init();

	pr_info(ps, "Perf-Studio (C)");
	pr_info(ps, "Version: %s", VERSION_STRING);

	/* intialize random subsystem - must be early */
	rand_init(ps);

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

	ret = load_projects_from_cache(ps);
	if (ret != 0) {
		err_msg(ps, "failed to load project configuration file");
		ret = EXIT_FAILURE;
		goto out;
	}


	ret = register_available_modules(ps);
	if (ret != 0) {
		err_msg(ps, "failed to register perf-studio modules");
		ret = EXIT_FAILURE;
		goto out2;
	}

	ret = gui_register_artwork(ps);
	if (ret != 0) {
		err_msg(ps, "failed to load artwork");
		ret = EXIT_FAILURE;
		goto out3;
	}

	register_signal_handler(ps);

	gui_init(ps, ac, av);

	gtk_main();


	ret = EXIT_SUCCESS;
out3:
	unregister_all_modules(ps);
out2:
	project_purge_all(ps);
out:
	rand_free(ps);
	ps_free(ps);
	pr_info(ps, "exiting");
	return ret;
}
