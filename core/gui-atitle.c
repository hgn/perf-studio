#include "gui-atitle.h"


void gui_atitle_set_title(struct ps *ps, const gchar *title)
{
	(void) title;

	// FIXME: get rid of markup and put in css file
	gtk_label_set_markup(GTK_LABEL(ps->s.atitle.label),
			"<span size=\"x-large\" font_weight=\"thin\" foreground=\"#666\">Project: Fooo  </span>");
}

void gui_atitle_set_tooltip(struct ps *ps, const gchar *tooltip)
{
	gtk_widget_set_tooltip_text(ps->s.atitle.label, tooltip);
}



void gui_atitle_init(struct ps *ps)
{
	GtkWidget *hbox;


	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ps->s.atitle.label = gtk_label_new(NULL);
	gui_atitle_set_title(ps, "Project Foo");
	gui_atitle_set_tooltip(ps, "\nDescription: nulla shizzlin dizzle Its fo rizzle faucibizzle pharetra\n"
			"Path: /usr/src/linux\n"
			"Args:\n");
	gtk_box_pack_end(GTK_BOX(hbox), ps->s.atitle.label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(ps->s.vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(ps->s.vbox);
}
