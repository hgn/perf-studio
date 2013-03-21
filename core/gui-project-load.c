
#include <string.h>
#include <assert.h>

#include "perf-studio.h"
#include "shared.h"
#include "gui-apo.h"
#include "gui-project-load.h"
#include "project.h"


static void screen_intro_dialog_existing_activated(GtkTreeView *view,
		GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data)
{
	int ret;
	gchar *id;
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct ps *ps;

	(void) col;
	ps = user_data;
	assert(ps);

	model = gtk_tree_view_get_model(view);
	if (!gtk_tree_model_get_iter(model, &iter, path)) {
		pr_error(ps, "Cannot get tree model");
		gtk_widget_destroy(ps->s.project_load_window);
		ps->s.project_load_window = NULL;
		return;
	}

	gtk_tree_model_get(model, &iter, 0, &id, -1);
	pr_info(ps, "project %s selected", id);
	ret = project_load_by_id(ps, id);
	if (ret != 0) {
		pr_error(ps, "Failed to load project %s", id);
		goto out;
	}

	/* inform APO panel */
	gui_apo_new_project_loaded(ps);

out:
	gtk_widget_destroy(ps->s.project_load_window);
	ps->s.project_load_window = NULL;
	g_free(id);
}


static void project_load_widget_add_project_list(struct ps *ps, GtkWidget *container)
{
	GtkListStore *lista1;
	GtkWidget *tree1;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	GSList *list_tmp;


	lista1 = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	tree1 = gtk_tree_view_new_with_model(GTK_TREE_MODEL(lista1));
	gtk_widget_show(tree1);

	g_signal_connect(tree1, "row-activated", G_CALLBACK(screen_intro_dialog_existing_activated), ps);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("  Project Id  ", renderer, "text", 0, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);
	column = gtk_tree_view_column_new_with_attributes("  Command  ", renderer, "text" , 1, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);
	column = gtk_tree_view_column_new_with_attributes("  Description  ", renderer, "text", 2, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);
	column = gtk_tree_view_column_new_with_attributes("  Last used  ", renderer, "text", 3, NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);

	list_tmp = ps->project_list;
	while (list_tmp) {
		struct project *project;
		project = list_tmp->data;
		gtk_list_store_append(lista1, &iter);
		gtk_list_store_set(lista1, &iter, 0, project->id,
						  1, project->cmd,
						  2, project->description,
						  3, "3 days ago",
						  -1);

		list_tmp = g_slist_next(list_tmp);
	}

        gtk_box_pack_start(GTK_BOX(container), tree1, FALSE, TRUE, 30);
        gtk_widget_show_all(tree1);
}


static void project_load_widget_add_header(struct ps *ps, GtkWidget *container)
{
	GtkWidget *hbox;
	GtkWidget *label;

	(void)ps;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new("Open Project");
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


void gui_amc_load_project(GtkWidget *widget, struct ps *ps)
{
	GtkWidget *vbox;

	(void)widget;

	ps->s.project_load_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(ps->s.project_load_window, 600, 400);
	gtk_container_set_border_width(GTK_CONTAINER(ps->s.project_load_window), 0);
	gtk_window_set_modal((GtkWindow *)ps->s.project_load_window, TRUE);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	project_load_widget_add_header(ps, vbox);
	project_load_widget_add_artwork(ps, vbox);
	project_load_widget_add_project_list(ps, vbox);
	gtk_widget_show(vbox);

	gtk_container_add(GTK_CONTAINER(ps->s.project_load_window), vbox);
	gtk_window_set_position((GtkWindow *)ps->s.project_load_window, GTK_WIN_POS_CENTER);
	gtk_window_present((GtkWindow *)ps->s.project_load_window);
	gtk_widget_show((GtkWidget *)ps->s.project_load_window);
	gtk_widget_grab_focus(GTK_WIDGET(ps->s.project_load_window));
}

