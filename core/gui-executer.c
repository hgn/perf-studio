#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "executer.h"
#include "shared.h"
#include "log.h"

struct executer_gui_priv_data {
	GtkWidget *main_window;
};


static GtkWidget *executer_main_content(struct executer_gui_ctx *executer_gui_ctx)
{
	GtkWidget *vbox;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	return vbox;
}


int executer_gui_init(struct executer_gui_ctx *executer_gui_ctx)
{
	struct executer_gui_priv_data *priv_data;
	GtkWidget *main_content;

	assert(executer_gui_ctx);
	assert(executer_gui_ctx->ps);
	assert(executer_gui_ctx->reply_cb);
	assert(!executer_gui_ctx->priv_data);

	log_print(LOG_DEBUG, "executer GUI now started, try to initialize subsystem");

	/* allocate our private data structure */
	priv_data = g_malloc0(sizeof(*priv_data));
	executer_gui_ctx->priv_data = priv_data;

	/* main windows setup */
	priv_data->main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(priv_data->main_window, 600, 400);
	gtk_container_set_border_width(GTK_CONTAINER(priv_data->main_window), 0);
	gtk_window_set_position(GTK_WINDOW(priv_data->main_window),
				GTK_WIN_POS_CENTER);
	gtk_window_set_modal((GtkWindow *)priv_data->main_window, TRUE);

	/* add main content */
	main_content = executer_main_content(executer_gui_ctx);
	if (!main_content) {
		log_print(LOG_ERROR, "Cannot init content");
		goto err;
	}

	/* show everything */
	gtk_widget_show((GtkWidget *)priv_data->main_window);
	gtk_widget_grab_focus(GTK_WIDGET(priv_data->main_window));

	return 0;

err:
	gtk_widget_destroy(priv_data->main_window);
	return -ENOMEM;
}


void executer_gui_free(struct executer_gui_ctx *executer_gui_ctx)
{
	struct executer_gui_priv_data *priv_data;

	assert(executer_gui_ctx);
	assert(executer_gui_ctx->ps);
	assert(executer_gui_ctx->priv_data);

	log_print(LOG_DEBUG, "executer GUI now released and resources freed");

	priv_data = executer_gui_ctx->priv_data;

	gtk_widget_destroy(priv_data->main_window);

	/* allocate our private data structure */
	g_free(executer_gui_ctx->priv_data);
	executer_gui_ctx->priv_data = NULL;
}


int executer_gui_update(struct executer_gui_ctx *executer_gui_ctx,
			struct executer_gui_update *executer_gui_update)
{
	assert(executer_gui_ctx);
	assert(executer_gui_update);

	log_print(LOG_DEBUG, "update GUI");

	return 0;
}
