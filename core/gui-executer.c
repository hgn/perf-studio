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


static void gui_add_title(GtkWidget *vbox)
{
	GtkWidget *hbox;
	GtkWidget *label;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new("Analyze Program");
	gtk_widget_set_name(label, "dialog_window_header");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(hbox);
}


static void gui_add_artwork(GtkWidget *vbox)
{
        GtkWidget *event_box;

        event_box = gtk_event_box_new();
	gtk_widget_set_name(event_box, "header");
	gtk_widget_set_size_request(event_box, -1, 10);

        gtk_box_pack_start(GTK_BOX(vbox), event_box, FALSE, TRUE, 0);
        gtk_widget_show_all(event_box);
}


static void gui_add_analyser_content(GtkWidget *vbox)
{
	GtkWidget *hbox;
	GtkWidget *entry;
	GtkWidget *progress_bar;
	GtkWidget *spinner;

	hbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	entry = gtk_entry_new();
	gtk_entry_set_text(GTK_ENTRY(entry), "Fooo");
	gtk_box_pack_start(GTK_BOX(hbox), entry, TRUE, FALSE, 0);

	progress_bar = gtk_progress_bar_new();
	gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progress_bar));
	//gtk_progress_bar_update(GTK_PROGRESS_BAR(progress_bar), 50);
	//gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar), 0.5);
	gtk_box_pack_start(GTK_BOX(hbox), progress_bar, TRUE, FALSE, 0);

	spinner = gtk_spinner_new();
	gtk_spinner_start(GTK_SPINNER(spinner));
	gtk_box_pack_start(GTK_BOX(hbox), spinner, TRUE, FALSE, 0);


	gtk_box_pack_start(GTK_BOX(vbox), hbox, TRUE, TRUE, 0);
	gtk_widget_show_all(vbox);
}


static void gui_add_button_bar(GtkWidget *vbox)
{
	GtkWidget *hbox;
	GtkWidget *cancel_button;
	GtkWidget *general_button;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	/* cancel button */
	cancel_button = gtk_button_new_with_label("Cancel");
	gtk_box_pack_start(GTK_BOX(hbox), cancel_button, TRUE, FALSE, 0);

	/* next button (start with start, next, et cetera) */
	general_button = gtk_button_new_with_label("Analyze Program");
	gtk_box_pack_end(GTK_BOX(hbox), general_button, TRUE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(hbox);
}


static GtkWidget *executer_main_content(struct executer_gui_ctx *executer_gui_ctx)
{
	GtkWidget *vbox;

	assert(executer_gui_ctx);

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gui_add_title(vbox);
	gui_add_artwork(vbox);
	gui_add_analyser_content(vbox);
	gui_add_button_bar(vbox);

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
	gtk_container_add(GTK_CONTAINER(priv_data->main_window), main_content);
	gtk_widget_show(main_content);

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
