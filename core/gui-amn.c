/* area module navigation */

#include <string.h>
#include <assert.h>

#include "perf-studio.h"
#include "gui-amn.h"
#include "gui-apo.h"
#include "gui-toolkit.h"
#include "shared.h"

enum {
	NAME = 0,
	ACTIVE,
	UPTODATE,
	SUGGESTED,
	NUM_COLS
};


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


static void screen_intro_dialog_existing_activated(GtkTreeView *view,
		GtkTreePath *path, GtkTreeViewColumn *col, gpointer user_data)
{
	gchar *name, *project_path;
	GtkTreeModel *model;
	GSList *list_tmp;
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

	gtk_tree_model_get(model, &iter, 0, &name, -1);
	gtk_tree_model_get(model, &iter, 1, &project_path, -1);

	list_tmp = ps->project_list;
	while (list_tmp) {
		struct project *project;
		project = list_tmp->data;

		if (streq(project->exec_path, name)) {
			// found project
			ps->project = project;
			pr_info(ps, "project selected: %s, path: %s\n",
				 name, project_path);
			break;
		}

		list_tmp = g_slist_next(list_tmp);
	}

	/* inform APO panel */
	gui_apo_new_project_loaded(ps);

	gtk_widget_destroy(ps->s.project_load_window);
	ps->s.project_load_window = NULL;
	g_free(name);
	g_free(project_path);
}


static void project_load_widget_add_project_list(struct ps *ps, GtkWidget *container)
{
	GtkListStore *lista1;
	GtkWidget *tree1;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeIter iter;
	GSList *list_tmp;


	lista1 = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	tree1 = gtk_tree_view_new_with_model(GTK_TREE_MODEL(lista1));
	gtk_widget_show(tree1);

	g_signal_connect(tree1, "row-activated", G_CALLBACK(screen_intro_dialog_existing_activated), ps);

	renderer = gtk_cell_renderer_text_new();
	column = gtk_tree_view_column_new_with_attributes("  Name  ", renderer, "text", 0,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);
	column = gtk_tree_view_column_new_with_attributes("  Path  ", renderer, "text" ,1,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);
	column = gtk_tree_view_column_new_with_attributes("  Last Used  ", renderer, "text",2,NULL);
	gtk_tree_view_append_column(GTK_TREE_VIEW(tree1), column);

	list_tmp = ps->project_list;
	while (list_tmp) {
		struct project *project;
		project = list_tmp->data;
		gtk_list_store_append(lista1, &iter);
		gtk_list_store_set(lista1, &iter, 0, project->exec_path,
						  1, project->exec_path,
						  2, project->exec_path,
						  -1);

		list_tmp = g_slist_next(list_tmp);
	}

        gtk_box_pack_start(GTK_BOX(container), tree1, FALSE, TRUE, 30);
        gtk_widget_show_all(tree1);
}


void gui_amc_load_project(GtkWidget *widget, struct ps *ps)
{
	GtkWidget *vbox;

	(void) widget;

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


static void row_activated(GtkTreeView *treeview, GtkTreePath *path,
		   GtkTreeViewColumn *col, gpointer priv_data)
{
#if 0
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct ps *ps;
	GList *tmp_list;

	assert(priv_data);

	ps = priv_data;

	pr_info(ps, "row activated");


	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		gchar *name;
		gtk_tree_model_get(model, &iter, NAME, &name, -1);

		fprintf(stderr, "row actived: %s\n", name);

		tmp_list = sc->control_pane_data_list;
		while (tmp_list != NULL) {
			struct control_pane_data *cpml;

			cpml = tmp_list->data;
			fprintf(stderr, "IN %s\n", cpml->name);

			if (!strcmp(cpml->name, name)) {
				GtkWidget *w;
				fprintf(stderr, "select and found: %s\n", name);
				w = cpml->widget_new(cpml->priv_data);
				main_notebook_add_widget(sc, cpml, w);
			}


			tmp_list = g_list_next(tmp_list);
		}

		g_free(name);
	}
#endif
}

static GtkTreeModel *create_and_fill_model(struct ps *ps)
{
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;

	(void)ps;

	treestore = gtk_tree_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

	gtk_tree_store_append(treestore, &toplevel, NULL);

	gtk_tree_store_set(treestore, &toplevel, NAME, "Core Analysis", -1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Time",
			-1);

	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel, NAME, "Application Level Analysis", UPTODATE, "yes", -1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Lock Contention Analysis",
			-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Scheduling Analysis",
			-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "CPU Bouncing",
			-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Priority Analysis",
			-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Wakeup Analysis",
			-1);



	gtk_tree_store_append(treestore, &toplevel, NULL);
	gtk_tree_store_set(treestore, &toplevel, NAME, "Architecture Level Analysis", -1);


	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Backend and Pipelining Stalls",
			-1);

	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "CPI/IPC",
			-1);
	gtk_tree_store_append(treestore, &child, &toplevel);
	gtk_tree_store_set(treestore, &child,
			NAME, "Cache Behavior",
			-1);


#if 0
	tmp = sc->control_pane_data_list;
	while (tmp != NULL) {
		struct control_pane_data *cpml;

		cpml = tmp->data;

		gtk_tree_store_append(treestore, &child, &toplevel);
		gtk_tree_store_set(treestore, &child, NAME, cpml->name, -1);

		tmp = g_list_next(tmp);
	}
#endif

	return GTK_TREE_MODEL(treestore);
}


static GtkWidget *create_view_and_model(struct ps *ps)
{
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	GtkWidget *view;
	GtkTreeModel *model;

	view = gtk_tree_view_new();

	/* column one */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, " Analysis ");

	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", NAME);


	/* column two */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, " Active ");

	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", ACTIVE);


	/* column three */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, " UpToDate ");

	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", UPTODATE);


	/* column four */
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, " Suggested ");

	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);

	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", SUGGESTED);


	model = create_and_fill_model(ps);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
	g_object_unref(model);

	gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(view)), GTK_SELECTION_NONE);

	return view;
}


GtkWidget *gui_amn_new(struct ps *ps)
{
	GtkWidget *scroll_widget;
	GtkWidget *view;

	scroll_widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_widget),
					    GTK_SHADOW_OUT);

	view = create_view_and_model(ps);
	g_signal_connect(view, "row-activated", G_CALLBACK(row_activated), ps);

	//selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget), view);

	return scroll_widget;
}
