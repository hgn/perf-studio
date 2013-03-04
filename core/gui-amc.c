/* area module content */

#include "gui-amc.h"
#include "gui-toolkit.h"

static void system_tab_init(struct ps *ps, GtkWidget *notebook)
{
        GtkWidget *frame;
        GtkWidget *label;

	frame = gtk_frame_new("Status Page");
	gtk_container_set_border_width(GTK_CONTAINER (frame), 2);
	gtk_widget_set_size_request (frame, 100, 75);
	gtk_widget_show(frame);

	label = gtk_label_new("System");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), frame, label);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), frame, TRUE);
}

GtkWidget *gui_amc_new(struct ps *ps)
{
	GtkWidget *notebook;

	notebook = gtk_notebook_new();
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);

	system_tab_init(ps, notebook);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
	gtk_widget_set_size_request(notebook, 50, -1);

	return notebook;
}


