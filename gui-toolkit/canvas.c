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

void gui_err_dialog(struct ps *ps, const gchar *format, ...)
{
	va_list  args;
	gchar *str;
	GtkWidget *dialog;

	va_start(args, format);
	str = g_strdup_vprintf (format, args);
	va_end(args);

	dialog = gtk_message_dialog_new(GTK_WINDOW(ps->s.main_window), 0,
				        GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", str);

	gtk_dialog_run(GTK_DIALOG(dialog));
	g_free(str);
	gtk_widget_destroy(dialog);
}
