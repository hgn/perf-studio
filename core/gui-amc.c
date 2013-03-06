/* area module content */

#include <assert.h>

#include "gui-amc.h"
#include "gui-toolkit.h"
#include "system-info.h"

#define CPU_USAGE_REFRESH 1000
#define CPU_USAGE_WIDTH_MAX 600
#define CPU_USAGE_HEIGHT_MAX 250


#define CPU_USAGE_TOWER_HEIGHT 100
#define CPU_USAGE_TOWER_WIDTH 30
#define CPU_USAGE_TOWER_MARGIN 10


#define TOP_SPACE 20
#define MARGIN_LEFT 40


static void draw_towers(struct ps *ps, GtkWidget *widget, cairo_t *cr, struct system_cpu *system_cpu)
{
	GSList *tmp;
	int pos_x, y_offset;

	(void)ps;

	pos_x = MARGIN_LEFT;

	cairo_set_line_width(cr, 0);

	int max_width = SYSTEM_CPU_NO_CPUS(system_cpu);
	max_width *= (CPU_USAGE_TOWER_WIDTH + CPU_USAGE_TOWER_MARGIN);
	max_width += CPU_USAGE_TOWER_WIDTH + 7 + MARGIN_LEFT;

	int width = gtk_widget_get_allocated_width(widget);
	int height = gtk_widget_get_allocated_height(widget);

	gdk_cairo_set_source_rgba(cr, &ps->si.color[BG_COLOR_DARKER]);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);

	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_width(cr, 1);

	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, MARGIN_LEFT - 10, TOP_SPACE);
	cairo_line_to(cr, MARGIN_LEFT - 10, TOP_SPACE + CPU_USAGE_TOWER_HEIGHT);
	cairo_stroke(cr);

	/* 100 % */
	cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
	cairo_move_to(cr, MARGIN_LEFT - 10 - 4, TOP_SPACE);
	cairo_line_to(cr, MARGIN_LEFT - 10, TOP_SPACE);
	cairo_stroke(cr);

	/* 75 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, MARGIN_LEFT - 10 - 4, TOP_SPACE + (CPU_USAGE_TOWER_HEIGHT * 0.25));
	cairo_line_to(cr, MARGIN_LEFT - 10, TOP_SPACE + (CPU_USAGE_TOWER_HEIGHT * 0.25));
	cairo_stroke(cr);

	/* 50 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, MARGIN_LEFT - 10 - 4, TOP_SPACE + (CPU_USAGE_TOWER_HEIGHT * 0.50));
	cairo_line_to(cr, MARGIN_LEFT - 10, TOP_SPACE + (CPU_USAGE_TOWER_HEIGHT * 0.50));
	cairo_stroke(cr);

	/* 25 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, MARGIN_LEFT - 10 - 4, TOP_SPACE + (CPU_USAGE_TOWER_HEIGHT * 0.75));
	cairo_line_to(cr, MARGIN_LEFT - 10, TOP_SPACE + (CPU_USAGE_TOWER_HEIGHT * 0.75));
	cairo_stroke(cr);

	/* 0 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, MARGIN_LEFT - 10 - 4, TOP_SPACE + CPU_USAGE_TOWER_HEIGHT);
	cairo_line_to(cr, MARGIN_LEFT - 10, TOP_SPACE + CPU_USAGE_TOWER_HEIGHT);
	cairo_stroke(cr);


	tmp = system_cpu->cpu_data_list;
	while (tmp) {
		PangoLayout *layout;
		float system_user_time, height;
		struct cpu_data *cpu_data;
		char buf[8];

		cpu_data = tmp->data;

		cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
		cairo_rectangle(cr, pos_x - 5, TOP_SPACE, CPU_USAGE_TOWER_WIDTH + 7, CPU_USAGE_TOWER_HEIGHT);
		cairo_fill(cr);

		/* draw data charts */
		//gdk_cairo_set_source_rgba(cr, &ps->si.color[BG_COLOR]);
		system_user_time = min(100.0f, cpu_data->system_time_percent +
				             cpu_data->user_time_percent);
		y_offset = TOP_SPACE + (100);
		height   = -system_user_time;
		cairo_set_source_rgb(cr, 0.122, 0.584, 0.714);
		cairo_rectangle(cr, pos_x, y_offset, CPU_USAGE_TOWER_WIDTH, height);
		cairo_fill(cr);

		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
		layout = create_pango_layout(cr);
		cairo_set_line_width(cr, 0);
		sprintf(buf, "CPU%d", cpu_data->cpu_no);

		pango_layout_set_text(layout, buf, -1);
		pango_cairo_update_layout(cr, layout);

		cairo_move_to(cr, pos_x - 5, 125);
		pango_cairo_show_layout(cr, layout);

		g_object_unref(layout);

		cairo_stroke_preserve(cr);
		cairo_restore(cr);


		pos_x += CPU_USAGE_TOWER_WIDTH + CPU_USAGE_TOWER_MARGIN;
		tmp = g_slist_next(tmp);
	}

	gtk_widget_set_size_request(widget, pos_x + CPU_USAGE_TOWER_WIDTH + 7, 150);
}


static gboolean draw_cb(GtkWidget *widget, GdkEventExpose *event)
{
	cairo_t *cr;
	struct ps *ps;
	struct system_cpu *system_cpu;

	(void) event;

	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);
	system_cpu = g_object_get_data(G_OBJECT(widget), "system-cpu");
	assert(system_cpu);


	//gtk_widget_set_size_request(widget, CPU_USAGE_WIDTH_MAX, rand() % 400);
	//width = gtk_widget_get_allocated_width(widget);
	//height = gtk_widget_get_allocated_height(widget);

	cr = gdk_cairo_create(gtk_widget_get_window(widget));

	draw_towers(ps, widget, cr, system_cpu);

	cairo_destroy(cr);

	return FALSE;
}


static gboolean configure_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	fprintf(stderr, "configure called\n");

	(void)widget;
	(void)event;
	(void)data;

	return FALSE;
}


static gboolean timer_cb(GtkWidget *widget)
{
	int width, height;
	struct system_cpu *sc;
	struct ps *ps;

	if (!gtk_widget_get_visible(widget)) {
		fprintf(stderr, "not visible\n");
		return TRUE;
	}

	/* get data first */
	sc = g_object_get_data(G_OBJECT(widget), "system-cpu");
	assert(sc);
	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);

	system_cpu_checkpoint(ps, sc);

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
	//gtk_widget_set_size_request(darea, CPU_USAGE_WIDTH_MAX, CPU_USAGE_HEIGHT_MAX);

	system_cpu = system_cpu_new(ps);

	g_signal_connect(darea, "draw", G_CALLBACK(draw_cb), NULL);
	g_signal_connect(darea, "configure-event", G_CALLBACK(configure_cb), NULL);

	g_object_set_data(G_OBJECT(darea), "ps", ps);
	g_object_set_data(G_OBJECT(darea), "system-cpu", system_cpu);
	g_timeout_add(CPU_USAGE_REFRESH, (GSourceFunc)timer_cb, darea);

	return darea;
}

void header_status_widget_set_title(GtkWidget *widget, const char *title)
{
	char buf[128];

	snprintf(buf, sizeof(buf) - 1,
			"<span size=\"large\" font_weight=\"thin\" "
			"foreground=\"#777\">%s</span>", title);

	gtk_label_set_markup(GTK_LABEL(widget), buf);
}

static GtkWidget *header_status_widget(struct ps *ps, const char *text)
{
	GtkWidget *hbox;
	GtkWidget *label;

	hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	label = gtk_label_new(NULL);
	header_status_widget_set_title(label, text);
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	label = gtk_label_new(NULL);
	header_status_widget_set_title(label, "Disable");
	gtk_box_pack_end(GTK_BOX(hbox), label, FALSE, FALSE, 0);

	return hbox;
}

static GtkWidget *system_tab_new(struct ps *ps)
{
	GtkWidget *vbox;
	GtkWidget *darea;
	GtkWidget *header;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = header_status_widget(ps, " CPU and Interrupt Info");
        gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);
	gtk_widget_show_all(header);

	darea = cpu_usage_new(ps);
        gtk_box_pack_start(GTK_BOX(vbox), darea, FALSE, TRUE, 0);
        gtk_widget_show_all(darea);

	header = header_status_widget(ps, " Disk and Slab Info");
        gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);
	gtk_widget_show_all(header);

	return vbox;
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


