/*
 * Area Module Navigation
 * Upper Left Area of Window
 */

#include <string.h>
#include <assert.h>

#include "perf-studio.h"
#include "gui-amn.h"
#include "gui-apo.h"
#include "gui-toolkit.h"
#include "shared.h"
#include "module-utils.h"
#include "module-loader.h"

enum {
	NAME = 0,
	ACTIVE,
	UPTODATE,
	SUGGESTED,
	NUM_COLS
};


static void module_selected_cb(GtkTreeView *treeview, GtkTreePath *path,
		   GtkTreeViewColumn *col, gpointer priv_data)
{
	GtkTreeModel *model;
	GtkTreeIter iter;
	struct ps *ps;

	assert(priv_data);
	ps = priv_data;

	(void)col;

	model = gtk_tree_view_get_model(treeview);

	if (gtk_tree_model_get_iter(model, &iter, path)) {
		gchar *name;
		gtk_tree_model_get(model, &iter, NAME, &name, -1);

		pr_debug(ps, "module panel row selected: %s", name);
		module_activated_by_name(ps, name);

		g_free(name);
	}
}


static void generate_tl_elements(GtkTreeStore *treestore, GtkTreeIter *root_elements)
{
	int i;
	const char *group_name;

	for (i = 0; i < MODULE_GROUP_MAX; i++) {
		group_name = module_group_str(i);
		gtk_tree_store_append(treestore, &root_elements[i], NULL);
		gtk_tree_store_set(treestore, &root_elements[i], NAME, group_name, -1);
	}
}


static void generate_cl_elements(GtkTreeStore *treestore,
				 GtkTreeIter *root, struct module *module)
{
	GtkTreeIter child;
	char *module_name;

	assert(treestore);
	assert(root);
	assert(module);

	module_name = module_get_name(module);

	gtk_tree_store_append(treestore, &child, root);
	gtk_tree_store_set(treestore, &child, NAME, module_name, -1);
}


static GtkTreeModel *create_and_fill_model(struct ps *ps)
{
	GtkTreeStore *treestore;
	GSList *tmp;
	GtkTreeIter root_elements[MODULE_GROUP_MAX];

	treestore = gtk_tree_store_new(NUM_COLS,
				       G_TYPE_STRING,
				       G_TYPE_STRING,
				       G_TYPE_STRING,
				       G_TYPE_STRING);

	generate_tl_elements(treestore, root_elements);

	tmp = ps->module_list;
	while (tmp) {
		GtkTreeIter *tree_iter;
		struct module *m = tmp->data;

		tree_iter = &root_elements[m->group];
		generate_cl_elements(treestore, tree_iter, m);

		tmp = g_slist_next(tmp);
	}


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
	g_signal_connect(view, "row-activated", G_CALLBACK(module_selected_cb), ps);

	//selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget), view);

	return scroll_widget;
}
