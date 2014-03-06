

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <gtk/gtk.h>

#include "perf-studio.h"
#include "shared.h"
#include "module-loader.h"
#include "conf-file.h"
#include "project.h"
#include "version.h"
#include "random.h"
#include "gui-main.h"
#include "log.h"
#include "executer.h"


static void print_usage(void)
{
	printf("perf-studio [options]\n");
	puts("Options:");
	puts("  -v, --verbose                verbose output");
	puts("  -t, --theme <light, dark>    select the used GUI theme");
	puts("  -h, --help                   print this output");
}


static int parse_cli_options(struct ps *ps, int ac, char **av)
{
	int c;

	ps->args.me = g_strdup(av[0]);
	/* FIXME: should be removed and get from config file */
	ps->args.theme = THEME_DARK;

	while (1) {
		int option_index = 0;
		static struct option long_options[] = {
			{"theme",            1, 0, 't'},
			{"verbose",          0, 0, 'v'},
			{"modules",          1, 0, 'm'},
			{"help",             0, 0, 'h'},
			{0, 0, 0, 0}
		};

		c = getopt_long(ac, av, "vmht:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 't':
			if (streq("dark", optarg)) {
				ps->args.theme = THEME_DARK;
			} else if (streq("light", optarg)) {
				ps->args.theme = THEME_LIGHT;
			} else {
				log_print(LOG_CRITICAL, "Theme not supported");
				exit(EXIT_FAILURE);
			}
			break;
		case 'v':
			ps->args.msg_level++;
			break;
		case 'm':
			ps->args.list_available_modules = TRUE;
			break;
		case 'h':
			print_usage();
			exit(EXIT_SUCCESS);
			break;
		case '?':
			break;

		default:
			err_msg(ps, "getopt returned character code 0%o ?", c);
			return -EINVAL;
		}
	}

	switch (ps->args.msg_level) {
	case 0:
		log_set_level(DEFAULT_MSG_LEVEL);
		break;
	case 1:
		log_set_level(LOG_DEBUG);
		break;
	default:
		log_set_level(LOG_DEBUG);
		log_set_verbose();
		break;
	}

	return 0;
}


static struct ps *ps_new(void)
{
	struct ps *ps;

	ps = g_malloc0(sizeof(struct ps));

	ps->screen_usable = FALSE;

	ps->args.list_available_modules = FALSE;
	ps->args.msg_level = 0;

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


static void sigterm_handler(int signal_no)
{
	assert(signal_no == SIGTERM);

	fprintf(stderr, " signal (SIGTERM) enforced exit\n");
	gtk_main_quit();
}


static void sigint_handler(int signal_no)
{
	assert(signal_no == SIGINT);

	fprintf(stderr, " signal (SIGINT) enforced exit\n");
	gtk_main_quit();
}


static void sigquit_handler(int signal_no)
{
	assert(signal_no == SIGQUIT);

	fprintf(stderr, "signal (SIGQUIT) enforced exit\n");
	gtk_main_quit();
}


static void sighup_handler(int signal_no)
{
	assert(signal_no == SIGHUP);

	fprintf(stderr, "signal (SIGHUP) enforced exit\n");
	gtk_main_quit();
}


static void register_signal_handler(struct ps *ps)
{
	int ret;
	struct sigaction sa;

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigterm_handler;
	ret = sigaction(SIGTERM, &sa, NULL);
	if (ret)
		err_msg_die(ps, EXIT_SYS_FAIL, "Cannot register sigterm handler");

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigquit_handler;
	ret = sigaction(SIGQUIT, &sa, NULL);
	if (ret)
		err_msg_die(ps, EXIT_SYS_FAIL, "Cannot register sigquit handler");

	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sigint_handler;
	ret = sigaction(SIGINT, &sa, NULL);
	if (ret)
		err_msg_die(ps, EXIT_SYS_FAIL, "Cannot register sigint handler");

	/* send by init during system shutdown */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = sighup_handler;
	ret = sigaction(SIGHUP, &sa, NULL);
	if (ret)
		err_msg_die(ps, EXIT_SYS_FAIL, "Cannot register SIGHUP handler");
}


int main (int ac, char **av)
{
	int ret;
	struct ps *ps;

	ps = ps_new();

	log_print(LOG_INFO, "Perf-Studio (C) - Version: %s", VERSION_STRING);

	/* intialize random subsystem - must be early */
	rand_init(ps);

	ret = parse_cli_options(ps, ac, av);
	if (ret != 0) {
		log_print(LOG_CRITICAL, "Failed to parse command line arguments");
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = executer_init(ps);
	if (ret != 0) {
		log_print(LOG_CRITICAL, "Failed to initialize executer");
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = load_user_conf_file(ps);
	if (ret != 0) {
		log_print(LOG_CRITICAL, "Failed to load configuration file");
		ret = EXIT_FAILURE;
		goto out;
	}

	ret = load_projects_from_cache(ps);
	if (ret != 0) {
		log_print(LOG_CRITICAL, "Failed to load project configuration file");
		ret = EXIT_FAILURE;
		goto out;
	}


	ret = register_available_modules(ps);
	if (ret != 0) {
		log_print(LOG_CRITICAL, "Failed to register perf-studio modules");
		ret = EXIT_FAILURE;
		goto out2;
	}

	ret = gui_register_artwork(ps);
	if (ret != 0) {
		log_print(LOG_CRITICAL, "Failed to load artwork");
		ret = EXIT_FAILURE;
		goto out3;
	}

	register_signal_handler(ps);

	gui_init(ps, ac, av);

	gtk_main();

	ret = EXIT_SUCCESS;
out3:
	executer_fini(ps);
out2:
	project_purge_all(ps);
out:
	rand_free(ps);
	unregister_all_modules(ps);
	log_print(LOG_INFO, "Exiting Perf Studio");
	ps_free(ps);
	return ret;
}
