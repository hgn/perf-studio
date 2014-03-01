#ifndef __BUILTIN_STUDIO_H
#define __BUILTIN_STUDIO_H

#include <linux/types.h>
#include <pthread.h>
#include <stdbool.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

//#include "studio-control-pane.h"

#include <assert.h>

enum {
	STUDIO_MODE_EXECUTION,
	STUDIO_MODE_IDLE,
};


struct studio_context {
	char *homedirpath;
	struct {
		GtkWidget *main_window;
		GtkItemFactory *main_menu;
		GtkWidget *vbox;

		GtkWidget *sw;

		GtkWidget *main_paned;
		GtkWidget *main_paned_control;
		GtkWidget *main_paned_workspace;

		GtkAccelGroup *accel_group;

		GtkWidget *statusbar;

		GtkWidget *dialog_window;

		/* notebook data */
		GtkWidget *schart;

		/* Modal window displayed if command
		 * is executed and sampling/counting
		 * is active
		 */
		GtkWidget *perf_run_window;

		/* small status widget display project
		 * name and active module */
		GtkWidget *status_widget;

		/* upper control widgets */
		GtkWidget *control_pane;

		GtkWidget *project_overview;

		int theme;
	} screen;

	struct {
		gchar *global_conf_path;
	} db;

	GList *control_pane_data_list;


	struct {
		GList *module_list;
	} modules;



	struct perf_project *perf_project_data;
	int mode;

	/* this is a function pointer
	 * list where every user can
	 * add himself. This function is
	 * called if the user press the execute
	 * button.
	 */
	GList *common_data_changed_cb_list;
	struct common_data *common_data;


	/* location to pixmaps and artwork */
	char *pixmapdir;
	char *buttondir;
};


#define MODULE_SPEC_MAX_NAME 32
#define MODULE_SPEC_MAX_DESCRIPTION 128

struct module_spec {

        char name[MODULE_SPEC_MAX_NAME];
        char description[MODULE_SPEC_MAX_DESCRIPTION];

        /* MODULE_CONTROL_CATEGORY_* */
        int module_control_category;

        /* module private data used
         * by the particular module and is passed to the
         * registered functions
         */
        void *priv_data;

        /* this function IS called before widget_new is called.
         * This mainly happens shortly after module registration
         * if a "old" trace file is found.
         * Is a new trace is started and the file is saved this
         * function is called again with the path to the new trace
         * file.
         *
         * is_trace_file_outdated(const char *trace_file_path)
         * can be used to test if a particular tracefile match
         * to the executable or is the trace outdated.
         */
        void (*new_trace_data_notifier)(void *priv_data, const char *trace_file_path);

        /* GUI section */

        /* widget_new is called if the module is selected and
         * will be added to the main notebook widget. The returned
         * widget must take care of resizing/scroll capabilities
         * and so on
         */
        GtkWidget *(*widget_new)(void *priv_data);

        /* This function is called if the widget is gone to be destroyed.
         * Widget specific data should be freed
         */
        void (*widget_destroy)(void *priv_data);
};

struct control_pane_data {
	char *name;
	GtkWidget *(*widget_new)(void *priv_data);
	void (*widget_destroy)(void *priv_data);
	void *priv_data;

	/* if a notebook is added to the notebook
	 * the this is the identifier required
	 * to delete it later */
	gint notebook_id;

	/* back  pointer */
	struct studio_context *sc;
};

/* studio-main-notebook.c */
GtkWidget *screen_notebook_main_init(struct studio_context *);
bool main_notebook_add_widget(struct studio_context *sc, struct control_pane_data *, GtkWidget *widget);


void screen_nootbook_thread_analyzer_init(GtkWidget *notebook);


/* studio-control-pane.c */
void module_overview_init(struct studio_context *sc);


struct control_pane_data *control_pane_data_by_name(struct studio_context *sc, const char *name);


/* studio-control-overview.c */
GtkWidget *screen_mainwindow_control_overview_new(struct studio_context *sc, gchar *path);


#endif	/* __BUILTIN_STUDIO_H*/

