#include "gui-about.h"
#include "version.h"
#include "authors.h"

#define PERF_LOGO_FILENAME "perf-studio-logo.png"

void gui_show_about(GtkWidget *widget, struct ps *ps)
{
	gchar *logo_path;
	GdkPixbuf *pixbuf;
	GtkWidget *about_dialog;

	(void) widget;

	logo_path = g_build_filename(ps->si.pixmapdir, PERF_LOGO_FILENAME, NULL);
	pixbuf = gdk_pixbuf_new_from_file(logo_path, NULL);

	about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about_dialog), pixbuf);
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), VERSION_STRING);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "(C) 2013 Hagen Paul Pfeifer <hagen@jauu.net>");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "http://perf-studio.com");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), perf_studio_authors);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about_dialog), GTK_LICENSE_GPL_3_0);


	g_free(logo_path);
	g_object_unref(pixbuf);
	gtk_window_set_position(GTK_WINDOW(about_dialog), GTK_WIN_POS_CENTER);
	gtk_dialog_run(GTK_DIALOG (about_dialog));
	gtk_widget_destroy(about_dialog);

}

