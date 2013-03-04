#include "gui-statusbar.h"

/*
 * Enable/disable via
 * gui.statusbar = <enabele | disable>
 */
void gui_statusbar_init(struct ps *ps)
{
	guint id;

	ps->s.statusbar = gtk_statusbar_new();

	g_object_set_data(G_OBJECT(ps->s.statusbar), "info", (gpointer) "1");
	g_object_set_data(G_OBJECT(ps->s.statusbar), "info", (gpointer) "2");
	g_object_set_data(G_OBJECT(ps->s.statusbar), "info", (gpointer) "3");

	/* stack for warning messages */
	g_object_set_data(G_OBJECT(ps->s.statusbar), "warning", (gpointer) "A");
	g_object_set_data(G_OBJECT(ps->s.statusbar), "warning", (gpointer) "B");
	g_object_set_data(G_OBJECT(ps->s.statusbar), "warning", (gpointer) "C");

	id = gtk_statusbar_get_context_id(GTK_STATUSBAR(ps->s.statusbar), "info");

	gtk_statusbar_push(GTK_STATUSBAR(ps->s.statusbar), id, "initialized");

	gtk_box_pack_start(GTK_BOX(ps->s.vbox), ps->s.statusbar, FALSE, FALSE, 0);
	gtk_widget_show(ps->s.statusbar);
}

