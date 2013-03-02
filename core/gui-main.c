#include <errno.h>
#include <assert.h>
#include <string.h>

#include "perf-studio.h"
#include "gui-main.h"
#include "shared.h"

static void init_styles(struct ps *ps)
{
	GdkDisplay *display;
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

	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);

	gtk_style_context_add_provider_for_screen(screen,
			GTK_STYLE_PROVIDER(provider),
			GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

out:
	g_object_unref(provider);
}


int gui_init(struct ps *ps, int ac, char **av)
{
	gtk_init(&ac, &av);

	init_styles(ps);

	ps->s.main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(ps->s.main_window), "perf-studio");


	/* gtk_main_quit() simple leave the main loop */
	g_signal_connect(ps->s.main_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(ps->s.main_window, "delete_event", G_CALLBACK(gtk_main_quit), NULL);

	gtk_container_set_border_width(GTK_CONTAINER(ps->s.main_window), 0);


	gtk_widget_show(ps->s.main_window);

	return 0;
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


	return 0;
}
