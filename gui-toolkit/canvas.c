#include "perf-studio.h"
#include "gui-toolkit.h"

GtkWidget *gt_stub_widget(struct ps *ps, const gchar *str, guint width, guint height)
{
	GtkWidget *frame;

	(void) ps;
	(void) width;
	(void) height;

	frame = gtk_frame_new(str);

	return frame;
}
