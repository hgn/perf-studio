/* area project overview */

#include "gui-apo.h"
#include "gui-toolkit.h"
#include "shared.h"


static void widget_set_title(GtkWidget *widget, const char *title)
{
	char buf[128];

	snprintf(buf, sizeof(buf) - 1,
			"<span size=\"x-large\" font_weight=\"thin\" "
			"foreground=\"#777\">%s</span>", title);

	gtk_label_set_markup(GTK_LABEL(widget), buf);
}

static GtkWidget *header_status_widget(struct ps *ps, const char *text)
{
	GtkWidget *hbox;
	GtkWidget *label;

	(void) ps;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new(NULL);
	widget_set_title(label, text);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	label = gtk_label_new(NULL);
	widget_set_title(label, "Disable");
	gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	return hbox;
}


static GtkWidget *project_info_widget_new(struct ps *ps)
{
	GtkWidget *grid;
	GtkWidget *l1, *l2;
	GtkWidget *e;

	(void) ps;

	grid = gtk_grid_new();

	/* spacing cells */
	l1 = gtk_label_new("      ");
	gtk_grid_attach(GTK_GRID(grid), l1, 0, 0, 1, 1);

	l2 = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), l2, 1, 0, 1, 1);

	l2 = gtk_label_new("     ");
	gtk_grid_attach(GTK_GRID(grid), l2, 2, 0, 1, 1);

	l2 = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), l2, 3, 0, 1, 1);



	l1 = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), l1, 0, 1, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Executable");
	gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, 1, 1, 1);

	l2 = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), l2, 2, 1, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Fooo");
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_widget_set_hexpand(e, TRUE);
	gtk_grid_attach(GTK_GRID(grid), e, 3, 1, 1, 1);



	l1 = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), l1, 0, 2, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(e), "Working Directory");
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, 2, 1, 1);

	l2 = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), l2, 2, 2, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(e), "Fooo");
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_widget_set_hexpand(e, TRUE);
	gtk_grid_attach(GTK_GRID (grid), e, 3, 2, 1, 1);


	return grid;
}


static GtkWidget *apo_main_widget_new(struct ps *ps)
{
	GtkWidget *vbox;
	GtkWidget *header;
	GtkWidget *project_info;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = header_status_widget(ps, " Project Overview");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);

	project_info = project_info_widget_new(ps);
	gtk_box_pack_start(GTK_BOX(vbox), project_info, FALSE, TRUE, 0);

	return vbox;
}


GtkWidget *gui_apo_new(struct ps *ps)
{
	GtkWidget *scroll_widget;
	GtkWidget *main_widget;


	scroll_widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_widget),
					    GTK_SHADOW_OUT);

	main_widget = apo_main_widget_new(ps);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget), main_widget);

	return scroll_widget;
}
