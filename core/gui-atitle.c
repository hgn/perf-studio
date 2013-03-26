#include <string.h>
#include <assert.h>

#include "gui-atitle.h"
#include "project.h"


static void gui_atitle_set_tooltip(struct ps *ps, const gchar *tooltip)
{
	gtk_widget_set_tooltip_text(ps->s.atitle.label, tooltip);
}


static void project_activated(struct ps *ps)
{
	const struct project *project;
	char buf[128];

	assert(ps->project);
	assert(ps->s.atitle.label);

	project = ps->project;
	snprintf(buf, sizeof(buf), "<span size=\"x-large\" font_weight=\"thin\""
			   "foreground=\"#666\">Project: %s  </span>", project->id);

	gtk_label_set_markup(GTK_LABEL(ps->s.atitle.label), buf);
}


static void project_deactivated(struct ps *ps)
{
	gtk_label_set_markup(GTK_LABEL(ps->s.atitle.label),
			"<span size=\"x-large\" font_weight=\"thin\" "
			"foreground=\"#666\">Project: None  </span>");
}


void gui_atitle_set_title(struct ps *ps, const gchar *title)
{
	(void) title;

	// FIXME: get rid of markup and put in css file
	gtk_label_set_markup(GTK_LABEL(ps->s.atitle.label),
			"<span size=\"x-large\" font_weight=\"thin\" "
			"foreground=\"#666\">Project: None  </span>");
}



void gui_atitle_init(struct ps *ps)
{
	GtkWidget *hbox;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	ps->s.atitle.label = gtk_label_new(NULL);
	/* we start with a "deactivated" project */
	project_deactivated(ps);
	gui_atitle_set_tooltip(ps, "\nDescription: nulla shizzlin dizzle Its"
			"fo rizzle faucibizzle pharetra\n"
			"Path: /usr/src/linux\n"
			"Args:\n");
	gtk_box_pack_end(GTK_BOX(hbox), ps->s.atitle.label, FALSE, FALSE, 0);

	gtk_box_pack_start(GTK_BOX(ps->s.vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show_all(ps->s.vbox);

	project_register_activate_cb(ps, project_activated);
	project_register_deactivate_cb(ps, project_deactivated);
}
