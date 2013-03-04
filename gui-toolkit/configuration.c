
#include "perf-studio.h"

void gt_set_widget_transparent(struct ps *ps, GtkWidget *w)
{
	char alpha_supported;
	GdkDisplay *display;
	GdkScreen *screen;
	GdkVisual *visual;

	display = gdk_display_get_default();
	screen = gdk_display_get_default_screen(display);

	visual = gdk_screen_get_rgba_visual(screen);
	if (visual == NULL)
		  visual = gdk_screen_get_system_visual(screen);

	alpha_supported =  visual ? 'y' : 'n';

	pr_info(ps, "set alpha transparency to widget (alpha channel supported: %c)\n", alpha_supported);
	gtk_widget_set_visual(GTK_WIDGET(w), visual);
}
