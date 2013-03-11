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

	return hbox;
}


static GtkWidget *project_info_widget_new(struct ps *ps)
{
	GtkWidget *grid;
	GtkWidget *label;
	GtkWidget *e;
	guint row;

	grid = gtk_grid_new();
	row = 0;

	/* spacing cells */
	label = gtk_label_new("  ");
	gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 1, row, 1, 1);

	label = gtk_label_new("     ");
	gtk_grid_attach(GTK_GRID(grid), label, 2, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 3, row, 1, 1);

	label = gtk_label_new("       ");
	gtk_grid_attach(GTK_GRID(grid), label, 4, row, 1, 1);
	row++;


	/* name */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Project Name");
	gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.name = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(ps->s.project_info.name), FALSE);
	gtk_entry_set_text(GTK_ENTRY(ps->s.project_info.name), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.name), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.name, TRUE);
	gtk_grid_attach(GTK_GRID(grid), ps->s.project_info.name, 3, row, 1, 1);
	row++;


	/* description */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Description");
	gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.description = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(ps->s.project_info.description), FALSE);
	gtk_entry_set_text(GTK_ENTRY(ps->s.project_info.description), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.description), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.description, TRUE);
	gtk_grid_attach(GTK_GRID(grid), ps->s.project_info.description, 3, row, 1, 1);
	row++;


	/* exec_path */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Execution Command");
	gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.exec_path = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(ps->s.project_info.exec_path), FALSE);
	gtk_entry_set_text(GTK_ENTRY(ps->s.project_info.exec_path), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.exec_path), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.exec_path, TRUE);
	gtk_grid_attach(GTK_GRID(grid), ps->s.project_info.exec_path, 3, row, 1, 1);
	row++;


	/* exec_args */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Arguments");
	gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.exec_args = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(ps->s.project_info.exec_args), FALSE);
	gtk_entry_set_text(GTK_ENTRY(ps->s.project_info.exec_args), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.exec_args), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.exec_args, TRUE);
	gtk_grid_attach(GTK_GRID(grid), ps->s.project_info.exec_args, 3, row, 1, 1);
	row++;

	/* working_dir */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 0, row, 1, 1);

	e = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(e), FALSE);
	gtk_entry_set_text(GTK_ENTRY(e), "Workding Directory");
	gtk_editable_set_editable(GTK_EDITABLE(e), FALSE);
	gtk_widget_set_name(GTK_WIDGET(e), "project_info_label");
	gtk_grid_attach(GTK_GRID (grid), GTK_WIDGET(e), 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID (grid), label, 2, row, 1, 1);

	ps->s.project_info.working_dir = gtk_entry_new();
	gtk_entry_set_overwrite_mode(GTK_ENTRY(ps->s.project_info.working_dir), FALSE);
	gtk_entry_set_text(GTK_ENTRY(ps->s.project_info.working_dir), "");
	gtk_widget_set_name(GTK_WIDGET(ps->s.project_info.working_dir), "project_info_label");
	gtk_widget_set_hexpand(ps->s.project_info.working_dir, TRUE);
	gtk_grid_attach(GTK_GRID(grid), ps->s.project_info.working_dir, 3, row, 1, 1);
	row++;

	/* spacing cells */
	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 1, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 2, row, 1, 1);

	label = gtk_label_new("");
	gtk_grid_attach(GTK_GRID(grid), label, 3, row, 1, 1);

	label = gtk_label_new(" ");
	gtk_grid_attach(GTK_GRID(grid), label, 4, row, 1, 1);


	return grid;
}


static GtkWidget *object_section_size_widget_new(void)
{
	GtkWidget *expander;
	GtkWidget *entry;

	expander= gtk_expander_new("Section Size");

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(expander), entry);
	gtk_expander_set_expanded(GTK_EXPANDER (expander), FALSE);

	return expander;
}


static GtkWidget *function_size_widget_new(void)
{
	GtkWidget *expander;
	GtkWidget *entry;

	expander= gtk_expander_new("Function Size");

	entry = gtk_entry_new();
	gtk_container_add(GTK_CONTAINER(expander), entry);
	gtk_expander_set_expanded(GTK_EXPANDER (expander), FALSE);

	return expander;
}


static GtkWidget *apo_main_widget_new(struct ps *ps)
{
	GtkWidget *vbox;
	GtkWidget *header;
	GtkWidget *project_info;
	GtkWidget *widget;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = header_status_widget(ps, " Project Overview");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 2);

	project_info = project_info_widget_new(ps);
	gtk_box_pack_start(GTK_BOX(vbox), project_info, FALSE, TRUE, 2);

	header = header_status_widget(ps, " Object Details");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 2);

	widget = object_section_size_widget_new();
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 4);

	widget = function_size_widget_new();
	gtk_box_pack_start(GTK_BOX(vbox), widget, FALSE, FALSE, 4);


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
