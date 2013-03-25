#include <errno.h>
#include <assert.h>
#include <string.h>

#include "perf-studio.h"
#include "shared.h"

#include "gui-main.h"
#include "gui-atitle.h"
#include "gui-amc.h"
#include "gui-amn.h"
#include "gui-apo.h"
#include "gui-statusbar.h"
#include "gui-toolkit.h"
#include "gui-about.h"
#include "gui-help.h"
#include "gui-menu.h"
#include "gui-project-load.h"


static GdkScreen *get_default_screen(void)
{
	return gdk_display_get_default_screen(gdk_display_get_default());
}


static void query_screen_info(struct ps *ps)
{
	GdkScreen *screen;

	screen = get_default_screen();
	ps->info.width = gdk_screen_get_width(screen);
	ps->info.height = gdk_screen_get_height(screen);
}


static void init_styles(struct ps *ps)
{
	GdkScreen *screen;
	gboolean ret;
	GtkCssProvider *provider;

	provider = gtk_css_provider_new();
	ret = gtk_css_provider_load_from_path(provider, ps->si.theme_style_path, NULL);
	if (!ret) {
		pr_error(ps, "Failed to load CSS provider for %s",
				ps->si.theme_style_path);
		goto out;
	}

	screen = get_default_screen();

	gtk_style_context_add_provider_for_screen(screen,
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

out:
	g_object_unref(provider);
}


static void setup_main_row_layout(struct ps *ps)
{
	ps->s.vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(ps->s.main_window), ps->s.vbox);
}


static void setup_main_row_artwork_image(struct ps *ps)
{
        GtkWidget *event_box;

        event_box = gtk_event_box_new();
	gtk_widget_set_name(event_box, "header");
	gtk_widget_set_size_request(event_box, 0, 30);

        gtk_box_pack_start(GTK_BOX(ps->s.vbox), event_box, FALSE, TRUE, 0);
        gtk_widget_show_all(event_box);
}


static void resize_main_window(struct ps *ps, GtkWidget *window)
{
	gtk_window_resize(GTK_WINDOW(window),
			  ps->info.width   * 5 / 6,
			  ps->info.height  * 3 / 4);
}


static GtkWidget *control_module_project_panel_new(struct ps *ps)
{
	GtkWidget *module_project_panel;
	GtkWidget *module_panel;
	GtkWidget *project_overview_panel;

	module_project_panel = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);

	module_panel = gui_amn_new(ps);
	gtk_widget_show_all(module_panel);
	gtk_paned_pack1(GTK_PANED(module_project_panel), module_panel, TRUE, TRUE);

	project_overview_panel = gui_apo_new(ps);
	gtk_paned_pack2(GTK_PANED(module_project_panel), project_overview_panel, TRUE, TRUE);

	return module_project_panel;
}


static void gui_main_content_pane_init(struct ps *ps)
{
	GtkWidget *main_panel;
	GtkWidget *upper_control_module_project_panel;
	GtkWidget *lower_module_panel;

	main_panel = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

	upper_control_module_project_panel = control_module_project_panel_new(ps);
	gtk_widget_show_all(upper_control_module_project_panel);
	gtk_paned_pack1(GTK_PANED(main_panel), upper_control_module_project_panel, TRUE, TRUE);

	lower_module_panel = gui_amc_new(ps);
	gtk_paned_pack2(GTK_PANED(main_panel), lower_module_panel, TRUE, TRUE);

	gtk_box_pack_start(GTK_BOX(ps->s.vbox), main_panel, TRUE, TRUE, 0);

	gtk_paned_set_position(GTK_PANED(main_panel), 300);

	gtk_widget_show_all(main_panel);
}


int gui_init(struct ps *ps, int ac, char **av)
{
	gtk_init(&ac, &av);

	ps->s.main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	init_styles(ps);

	query_screen_info(ps);

	gtk_window_set_title(GTK_WINDOW(ps->s.main_window), "perf-studio");


	/* gtk_main_quit() simple leave the main loop */
	g_signal_connect(ps->s.main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(ps->s.main_window, "delete_event", G_CALLBACK(gtk_main_quit), NULL);

	gtk_container_set_border_width(GTK_CONTAINER(ps->s.main_window), 0);

	/* main layout */
	setup_main_row_layout(ps);

	/* first row: upper image */
	setup_main_row_artwork_image(ps);

	/* next row: main menubar */
	gui_menu_init(ps);

	/* next row: project title on the right */
	gui_atitle_init(ps);

	/* next row main widget where all content widgets are stored */
	gui_main_content_pane_init(ps);

	/* finally at a statusbar at the bottom of the window */
	if (ps->conf.ui.statusbar_enabled) {
		gui_statusbar_init(ps);
	}


	resize_main_window(ps, ps->s.main_window);
	gtk_window_set_position(GTK_WINDOW(ps->s.main_window), GTK_WIN_POS_CENTER);

	gtk_widget_show_all(ps->s.main_window);

	return 0;
}


static void colors_init(struct ps *ps)
{
	pr_info(ps, "Initialize color subsystem");

	switch (ps->args.theme) {
	case THEME_DARK:
		gdk_rgba_parse(&ps->si.color[FG_COLOR], "#eeeeee");
		gdk_rgba_parse(&ps->si.color[BG_COLOR], "#262626");
		gdk_rgba_parse(&ps->si.color[BG_COLOR_DARKER], "#161616");
		break;
	case THEME_LIGHT:
		gdk_rgba_parse(&ps->si.color[FG_COLOR], "#111111");
		gdk_rgba_parse(&ps->si.color[BG_COLOR], "#eeeeee");
		gdk_rgba_parse(&ps->si.color[BG_COLOR_DARKER], "#bbbbbb");
		break;
	}
}


int gui_register_artwork(struct ps *ps)
{
	int ret;
	char image_path[PATH_MAX];

	switch (ps->args.theme) {
	case THEME_DARK:
		ps->si.pixmapdir = g_strdup_printf("%s/%s",
				DATA_DIR, "artwork/pixmaps/dark/");
		ps->si.buttondir = g_strdup_printf("%s/%s",
				DATA_DIR, "artwork/pixmaps/buttons/16x16/");
		ps->si.theme_style_path = g_strdup_printf("%s/%s",
				DATA_DIR, "artwork/theme-style/dark/theme.css");
		break;
	case THEME_LIGHT:
		ps->si.pixmapdir = g_strdup_printf("%s/%s",
				DATA_DIR, "artwork/pixmaps/dark/");
		ps->si.buttondir = g_strdup_printf("%s/%s",
				DATA_DIR, "artwork/pixmaps/buttons/16x16/");
		ps->si.theme_style_path = g_strdup_printf("%s/%s",
				DATA_DIR, "artwork/theme-style/light/theme.css");
		break;
	default:
		assert(0);
	}

	/* make a accesstest */
	ret = snprintf(image_path, sizeof(image_path), "%s%s",  ps->si.pixmapdir, "header.png");
	if (ret < (int)strlen("back.png")) {
		pr_error(ps, "Cannot construct path to pixmap images");
		return -EINVAL;
	}

	if (access(image_path, F_OK)) {
		pr_error(ps, "Image directory seems empty: %s", ps->si.pixmapdir);
		pr_error(ps, "Did do call \"make install\"?");
		return -EINVAL;
	}

	pr_info(ps, "Pixmaps path: %s", ps->si.pixmapdir);
	pr_info(ps, "Button path:  %s", ps->si.buttondir);
	pr_info(ps, "Theme path:   %s", ps->si.theme_style_path);

	colors_init(ps);

	return 0;
}
