/* area module content */

#include <assert.h>

#include "gui-amc.h"
#include "gui-toolkit.h"
#include "system-info.h"

#define CPU_USAGE_REFRESH 1000
#define CPU_USAGE_WIDTH_MAX 600
#define CPU_USAGE_HEIGHT_MAX 250


static gboolean draw_cb(GtkWidget *widget, GdkEventExpose *event)
{
	cairo_t *cr;
	guint width, height;
	GdkRGBA color, color2;

	gdk_rgba_parse(&color, "#000000");
	gdk_rgba_parse(&color2, "#222222");

	width = gtk_widget_get_allocated_width (widget);
	height = gtk_widget_get_allocated_height (widget);

	cr = gdk_cairo_create(gtk_widget_get_window(widget));
	gdk_cairo_set_source_rgba(cr, &color2);
	cairo_paint(cr);
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_set_line_width(cr, 2);
	cairo_rectangle(cr, 0, 0, width - 10, height - 10);
	cairo_stroke(cr);

	cairo_destroy(cr);

	return FALSE;
}


static gboolean configure_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	fprintf(stderr, "configure called\n");

	return FALSE;
}


static gboolean timer_cb(GtkWidget *widget)
{
	int width, height;
	struct system_cpu *sc;
	struct system_cpu_info sci;
	struct ps *ps;

	fprintf(stderr, "timer called %p\n", g_object_get_data(G_OBJECT(widget), "ev"));

	if (!gtk_widget_get_visible(widget)) {
		fprintf(stderr, "not visible\n");
		return TRUE;
	}

	/* get data first */
	sc = g_object_get_data(G_OBJECT(widget), "system-cpu");
	assert(sc);
	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);

	system_cpu_checkpoint(ps, sc, &sci);

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	gtk_widget_queue_draw_area(widget, 0, 0, width, height);

	return TRUE;
}


static GtkWidget *cpu_usage_new(struct ps *ps)
{
	GtkWidget *darea;
	struct system_cpu *system_cpu;

	darea = gtk_drawing_area_new();
	gtk_widget_set_size_request(darea, CPU_USAGE_WIDTH_MAX, CPU_USAGE_HEIGHT_MAX);

	system_cpu = system_cpu_new(ps);
	system_cpu_start(ps, system_cpu);

	g_signal_connect(darea, "draw", G_CALLBACK(draw_cb), NULL);
	g_signal_connect(darea, "configure-event", G_CALLBACK(configure_cb), NULL);

	g_object_set_data(G_OBJECT(darea), "ps", ps);
	g_object_set_data(G_OBJECT(darea), "system-cpu", system_cpu);
	g_timeout_add(CPU_USAGE_REFRESH, (GSourceFunc)timer_cb, darea);

	return darea;
}


static GtkWidget *system_tab_new(struct ps *ps)
{
	GtkWidget *grid;
	GtkWidget *darea;

	grid = gtk_grid_new();
	darea = cpu_usage_new(ps);

	gtk_grid_attach(GTK_GRID(grid), darea, 0, 1, 1, 2);

	return grid;
}


static void system_tab_init(struct ps *ps, GtkWidget *notebook)
{
        GtkWidget *frame;
        GtkWidget *label;

	frame = system_tab_new(ps);
	gtk_container_set_border_width(GTK_CONTAINER (frame), 2);
	//gtk_widget_set_size_request (frame, 100, 75);
	gtk_widget_show(frame);

	label = gtk_label_new("System");
	gtk_notebook_append_page(GTK_NOTEBOOK (notebook), frame, label);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), frame, TRUE);
}


GtkWidget *gui_amc_new(struct ps *ps)
{
	GtkWidget *notebook;

	notebook = gtk_notebook_new();
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook), GTK_POS_BOTTOM);

	system_tab_init(ps, notebook);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
	gtk_widget_set_size_request(notebook, 50, -1);

	return notebook;
}


