#include "gui-about.h"
#include "version.h"
#include "authors.h"

void gui_show_about(GtkWidget *widget, struct ps *ps)
{
	GtkWidget *about_dialog;

	(void) widget;
	(void) ps;

	about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), VERSION_STRING);
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "(C) 2013 Hagen Paul Pfeifer <hagen@jauu.net>");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "http://perf-studio.com");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), perf_studio_authors);
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(about_dialog), GTK_LICENSE_GPL_3_0);


	gtk_dialog_run(GTK_DIALOG (about_dialog));
	gtk_widget_destroy(about_dialog);
}

