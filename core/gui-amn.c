/* area module navigation */

#include <assert.h>

#include "perf-studio.h"
#include "gui-amn.h"
#include "gui-toolkit.h"
#include "shared.h"

enum {
	NAME = 0,
	ACTIVE,
	UPTODATE,
	SUGGESTED,
	NUM_COLS
};


void row_activated(GtkTreeView *treeview, GtkTreePath *path,
		   GtkTreeViewColumn *col, gpointer priv_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct ps *ps;
	GList *tmp_list;

	assert(priv_data);

	ps = priv_data;

	pr_info(ps, "row activated");

#if 0

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
	GList *tmp;
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;

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
	GtkTreeSelection *selection;

	scroll_widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_widget),
					    GTK_SHADOW_OUT);

	view = create_view_and_model(ps);
	g_signal_connect(view, "row-activated", G_CALLBACK(row_activated), ps);

	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget), view);

	return scroll_widget;
}
