
#include <string.h>
#include <assert.h>

#include "perf-studio.h"
#include "shared.h"
#include "gui-apo.h"
#include "gui-project-load.h"
#include "gui-event-generator.h"
#include "project.h"


static void project_load_widget_add_header(struct ps *ps, GtkWidget *container)
{
	GtkWidget *hbox;
	GtkWidget *label;

	(void)ps;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new("Execute and Analyse Command");
	gtk_widget_set_name(label, "dialog_window_header");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(container), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(hbox);
}


static void project_load_widget_add_artwork(struct ps *ps, GtkWidget *container)
{
        GtkWidget *event_box;

	(void)ps;

        event_box = gtk_event_box_new();
	gtk_widget_set_name(event_box, "header");
	gtk_widget_set_size_request(event_box, -1, 10);

        gtk_box_pack_start(GTK_BOX(container), event_box, FALSE, TRUE, 0);
        gtk_widget_show_all(event_box);
}


static void project_load_widget_add_command_data(struct ps *ps, GtkWidget *container)
{
	GtkWidget *hbox;
	GtkWidget *label;
	struct project *project;

	project = ps->project;
	assert(project);

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new(project->cmd);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(container), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(hbox);
}


static void project_load_widget_add_spacer(GtkWidget *container)
{
	GtkWidget *spacer;
	spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_box_pack_start(GTK_BOX(container), spacer, TRUE, FALSE, 0);
	gtk_widget_show_all(spacer);
}


static void project_load_widget_add_cancel_start_button(struct ps *ps, GtkWidget *container)
{
	GtkWidget *hbox;
	GtkWidget *button_cancel;
	GtkWidget *button_start;

	(void)ps;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	button_cancel = gtk_button_new();
	gtk_button_set_label(GTK_BUTTON(button_cancel), "Cancel");
	gtk_widget_set_name(button_cancel, "dialog_window_button");
	gtk_box_pack_start(GTK_BOX(hbox), button_cancel, FALSE, FALSE, 10);

	button_start = gtk_button_new();
	gtk_button_set_label(GTK_BUTTON(button_start), "Start");
	gtk_widget_set_name(button_start, "dialog_window_button");
	gtk_box_pack_end(GTK_BOX(hbox), button_start, FALSE, FALSE, 10);

	gtk_box_pack_start(GTK_BOX(container), hbox, FALSE, FALSE, 5);
	gtk_widget_show_all(hbox);
}


void gui_event_executer_setup(struct ps *ps, struct project *project)
{
	GtkWidget *vbox;

	(void)project;

	ps->s.event_executer_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(ps->s.event_executer_window, 600, 400);
	gtk_container_set_border_width(GTK_CONTAINER(ps->s.event_executer_window), 0);
	gtk_window_set_modal((GtkWindow *)ps->s.event_executer_window, TRUE);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	project_load_widget_add_header(ps, vbox);
	project_load_widget_add_artwork(ps, vbox);
	project_load_widget_add_command_data(ps, vbox);
	project_load_widget_add_spacer(vbox);
	project_load_widget_add_cancel_start_button(ps, vbox);
	gtk_widget_show(vbox);

	gtk_container_add(GTK_CONTAINER(ps->s.event_executer_window), vbox);
	gtk_window_set_position((GtkWindow *)ps->s.event_executer_window, GTK_WIN_POS_CENTER);
	gtk_window_present((GtkWindow *)ps->s.event_executer_window);
	gtk_widget_show((GtkWidget *)ps->s.event_executer_window);
	gtk_widget_grab_focus(GTK_WIDGET(ps->s.event_executer_window));
}

