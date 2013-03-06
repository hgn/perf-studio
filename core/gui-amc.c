/* area module content */

#include <assert.h>

#include "gui-amc.h"
#include "gui-toolkit.h"
#include "system-info.h"

#define CPU_USAGE_REFRESH 1000

#define CPU_USAGE_CHART_WIDTH 30
#define CPU_USAGE_CHART_HEIGHT 250
#define CPU_USAGE_CHART_INNER_GAP 4
#define CPU_USAGE_CHART_GAP 10
#define CPU_USAGE_MARGIN_TOP 20
#define CPU_USAGE_MARING_LEFT 30
#define CPU_USAGE_CPU_LABEL_HEIGHT 40
#define CPU_USAGE_AXIS_MARGIN 5
#define CPU_USAGE_AXIS_LINE_LENGTH 4
#define CPU_USAGE_AXIS_DESC_WIDTH 50

#define CPU_USAGE_CHART_AXIS_HEIGHT(x) (CPU_USAGE_MARGIN_TOP + (CPU_USAGE_CHART_HEIGHT * x))
#define CPU_USAGE_CHART_AXIS_HEIGHT_100 CPU_USAGE_MARGIN_TOP
#define CPU_USAGE_CHART_AXIS_HEIGHT_75  CPU_USAGE_CHART_AXIS_HEIGHT(0.25f)
#define CPU_USAGE_CHART_AXIS_HEIGHT_50  CPU_USAGE_CHART_AXIS_HEIGHT(0.50f)
#define CPU_USAGE_CHART_AXIS_HEIGHT_25  CPU_USAGE_CHART_AXIS_HEIGHT(0.75f)
#define CPU_USAGE_CHART_AXIS_HEIGHT_0   (CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT)


#define CPU_USAGE_SYS_USER_USAGE_GAP 40

#define SYS_USER_USAGE_CHART_WIDTH 30
#define SYS_USER_USAGE_CHART_HEIGHT 200
#define SYS_USER_USAGE_CHART_INNER_GAG 2
#define SYS_USER_USAGE_MARGIN_TOP 20


static void draw_cpu_usage_background(struct ps *ps, GtkWidget *widget, cairo_t *cr)
{
	int width, height;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	gdk_cairo_set_source_rgba(cr, &ps->si.color[BG_COLOR_DARKER]);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);
}


static void draw_cpu_usage_axis(cairo_t *cr, int x_position)
{
	int axis_x_start;
	int axis_x_end;

	axis_x_start = x_position + CPU_USAGE_AXIS_MARGIN;
	axis_x_end = axis_x_start + CPU_USAGE_AXIS_LINE_LENGTH;

	/* draw axis */
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_width(cr, 1);
	cairo_set_source_rgb(cr, 0.25, 0.25, 0.25);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_MARGIN_TOP);
	cairo_line_to(cr, axis_x_start, CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT);
	cairo_stroke(cr);

	/* 100 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_100 + 1);
	cairo_line_to(cr, axis_x_end, CPU_USAGE_CHART_AXIS_HEIGHT_100 + 1);
	cairo_stroke(cr);

	/* 75 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_75);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_75);
	cairo_stroke(cr);

	/* 50 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_50);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_50);
	cairo_stroke(cr);

	/* 25 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_25);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_25);
	cairo_stroke(cr);

	/* 0 % */
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_0);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_0);
	cairo_stroke(cr);
}


static void draw_cpu_usage_charts(struct ps *ps, GtkWidget *widget,
				  cairo_t *cr, struct system_cpu *system_cpu)
{
	int x_position;
	GSList *tmp;

	cairo_set_line_width(cr, 0);

	draw_cpu_usage_background(ps, widget, cr);

	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

	unsigned long no_cpu = SYSTEM_CPU_NO_CPUS(system_cpu);

	x_position = CPU_USAGE_MARING_LEFT;

	tmp = system_cpu->cpu_data_list;
	while (tmp) {
		PangoLayout *layout;
		struct cpu_data *cpu_data;
		int system_user_time, height;
		char buf[8];

		cpu_data = tmp->data;

		/* draw background rectangle */
		cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
		cairo_rectangle(cr, x_position, CPU_USAGE_MARGIN_TOP,
				CPU_USAGE_CHART_WIDTH, CPU_USAGE_CHART_HEIGHT);
		cairo_fill(cr);

		/* draw chart */
		system_user_time = min(100.0f, cpu_data->system_time_percent +
				               cpu_data->user_time_percent);

		/* no need to draw if CPU is down */
		height = -((float)CPU_USAGE_CHART_HEIGHT * (system_user_time / 100.0f));
		cairo_set_source_rgb(cr, 0.122, 0.584, 0.714);
		cairo_rectangle(cr,
				x_position + CPU_USAGE_CHART_INNER_GAP,
				CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT,
				CPU_USAGE_CHART_WIDTH - (CPU_USAGE_CHART_INNER_GAP * 2),
				height);
		cairo_fill(cr);

		/* CPUn */
		cairo_save(cr);
		cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
		layout = create_pango_layout(cr);
		cairo_set_line_width(cr, 0);
		sprintf(buf, "CPU%d", cpu_data->cpu_no);

		pango_layout_set_text(layout, buf, -1);
		pango_cairo_update_layout(cr, layout);

		cairo_move_to(cr, x_position, CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT + 5);
		pango_cairo_show_layout(cr, layout);

		g_object_unref(layout);

		cairo_stroke_preserve(cr);
		cairo_restore(cr);

		x_position += CPU_USAGE_CHART_WIDTH + CPU_USAGE_CHART_GAP;
		tmp = g_slist_next(tmp);
	}

	/* correct x possition by wrong gap */
	x_position -= CPU_USAGE_CHART_GAP;

	draw_cpu_usage_axis(cr, x_position);


	gtk_widget_set_size_request(widget,
			x_position + CPU_USAGE_CHART_WIDTH,
			CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT + CPU_USAGE_CPU_LABEL_HEIGHT);
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

	draw_cpu_usage_charts(ps, widget, cr, system_cpu);

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

	header = header_status_widget(ps, " Memory and Slab Info");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);
	gtk_widget_show_all(header);

	header = header_status_widget(ps, " SoftIRQ");
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


