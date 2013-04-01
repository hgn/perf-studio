/* area module content */

#include <assert.h>
#include <math.h>
#include <float.h>

#include "gui-amc.h"
#include "gui-toolkit.h"
#include "gui-waterfall.h"
#include "system-info.h"
#include "shared.h"

#if 0
inner: padding
outer: margin
#endif

#define CPU_USAGE_REFRESH 1000

#define CPU_USAGE_CHART_WIDTH 30
#define CPU_USAGE_CHART_HEIGHT 250
#define CPU_USAGE_CHART_INNER_GAP 4
#define CPU_USAGE_CHART_GAP 10
#define CPU_USAGE_MARGIN_TOP 40
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


#define CPU_USAGE_SYS_USER_USAGE_GAP 100

#define SYS_USER_USAGE_CHART_WIDTH 30
#define SYS_USER_USAGE_CHART_HEIGHT CPU_USAGE_CHART_HEIGHT
#define SYS_USER_USAGE_CHART_INNER_GAG 5
#define SYS_USER_USAGE_MARGIN_TOP CPU_USAGE_MARGIN_TOP


#define MARGIN_TOP 20
#define PADDING_TOP 10

#define PADDING_LEFT 20

enum {
	SYSTEM_TAB_CPU_WIDGET,
	SYSTEM_TAB_INTR_WIDGET,
	SYSTEM_TAB_MEM_WIDGET,

	SYSTEM_TAB_MAX
};


static void draw_user_vs_system_axis(cairo_t *cr, int x_position)
{
	PangoLayout *layout;
	int axis_x_start;
	int axis_x_end;

	cairo_save(cr);
	layout = create_pango_layout(cr, "Sans 7");
	cairo_set_source_rgb(cr, 0.25, 0.25, 0.25);

	pango_layout_set_text(layout, "User versus System Time", -1);
	cairo_move_to(cr, x_position - 55, 17);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	g_object_unref(layout);
	layout = create_pango_layout(cr, "Sans 5");

	axis_x_start = x_position + CPU_USAGE_AXIS_MARGIN;
	axis_x_end   = axis_x_start + CPU_USAGE_AXIS_LINE_LENGTH;


	/* draw axis */
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_width(cr, 1);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_MARGIN_TOP);
	cairo_line_to(cr, axis_x_start, CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT);
	cairo_stroke(cr);

	/* 100 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_100 + 1);
	cairo_line_to(cr, axis_x_end, CPU_USAGE_CHART_AXIS_HEIGHT_100 + 1);
	cairo_stroke(cr);

	pango_layout_set_text(layout, "100%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_100 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	/* 75 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_75);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_75);
	cairo_stroke(cr);

	pango_layout_set_text(layout, " 75%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_75 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);


	/* 50 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_50);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_50);
	cairo_stroke(cr);

	pango_layout_set_text(layout, " 50%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_50 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	/* 25 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_25);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_25);
	cairo_stroke(cr);

	pango_layout_set_text(layout, " 25%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_25 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	/* 0 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_0);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_0);
	cairo_stroke(cr);

	pango_layout_set_text(layout, "  0%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_0 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	g_object_unref(layout);
	cairo_restore(cr);
}


static int draw_user_vs_system_char(cairo_t *cr, int x_position,
				    float acc_system, float acc_user)
{
	int height;
	float ratio;

	if (acc_system < FLT_EPSILON)
		acc_system = FLT_MIN;

	if (acc_user < FLT_EPSILON)
		acc_user = FLT_MIN;

	ratio = acc_user / (acc_user + acc_system);

	x_position += CPU_USAGE_SYS_USER_USAGE_GAP;

	/* background rectangle */
	cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
	cairo_rectangle(cr, x_position,
			SYS_USER_USAGE_MARGIN_TOP - 30,
			SYS_USER_USAGE_CHART_WIDTH + 100,
			SYS_USER_USAGE_MARGIN_TOP + SYS_USER_USAGE_CHART_HEIGHT + 10);
	cairo_fill(cr);

	x_position += PADDING_LEFT;

	cairo_set_source_rgb(cr, 0.2, 0.2, 0.2);
	cairo_rectangle(cr, x_position, SYS_USER_USAGE_MARGIN_TOP,
			SYS_USER_USAGE_CHART_WIDTH, SYS_USER_USAGE_CHART_HEIGHT);
	cairo_fill(cr);

	/* draw chart */
	height = -((float)SYS_USER_USAGE_CHART_HEIGHT * (ratio));
	cairo_set_source_rgb(cr, 0.122, 0.584, 0.714);
	cairo_rectangle(cr,
			x_position + SYS_USER_USAGE_CHART_INNER_GAG,
			SYS_USER_USAGE_MARGIN_TOP + SYS_USER_USAGE_CHART_HEIGHT,
			SYS_USER_USAGE_CHART_WIDTH - (SYS_USER_USAGE_CHART_INNER_GAG * 2),
			height);
	cairo_fill(cr);

	draw_user_vs_system_axis(cr, x_position + SYS_USER_USAGE_CHART_WIDTH);

	return x_position - 30 + SYS_USER_USAGE_CHART_WIDTH + 100;
}


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
	PangoLayout *layout;
	int axis_x_start;
	int axis_x_end;

	cairo_save(cr);
	layout = create_pango_layout(cr, "Sans 5");

	axis_x_start = x_position + CPU_USAGE_AXIS_MARGIN;
	axis_x_end = axis_x_start + CPU_USAGE_AXIS_LINE_LENGTH;

	cairo_set_source_rgb(cr, 0.25, 0.25, 0.25);

	/* draw axis */
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
	cairo_set_line_width(cr, 1);
	cairo_move_to(cr, axis_x_start, CPU_USAGE_MARGIN_TOP);
	cairo_line_to(cr, axis_x_start, CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT);
	cairo_stroke(cr);

	/* 100 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_100 + 1);
	cairo_line_to(cr, axis_x_end, CPU_USAGE_CHART_AXIS_HEIGHT_100 + 1);
	cairo_stroke(cr);

	pango_layout_set_text(layout, "100%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_100 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	/* 75 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_75);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_75);
	cairo_stroke(cr);

	pango_layout_set_text(layout, " 75%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_75 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);


	/* 50 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_50);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_50);
	cairo_stroke(cr);

	pango_layout_set_text(layout, " 50%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_50 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	/* 25 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_25);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_25);
	cairo_stroke(cr);

	pango_layout_set_text(layout, " 25%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_25 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);

	/* 0 % */
	cairo_move_to(cr, axis_x_start, CPU_USAGE_CHART_AXIS_HEIGHT_0);
	cairo_line_to(cr, axis_x_end,   CPU_USAGE_CHART_AXIS_HEIGHT_0);
	cairo_stroke(cr);

	pango_layout_set_text(layout, "  0%", -1);
	cairo_move_to(cr, axis_x_start + 8, CPU_USAGE_CHART_AXIS_HEIGHT_0 - 4);
	pango_cairo_show_layout(cr, layout);
	cairo_stroke(cr);


	g_object_unref(layout);
	cairo_restore(cr);
}


static int draw_cpu_usage_chart(struct ps *ps, GtkWidget *widget,
		cairo_t *cr, struct system_cpu *system_cpu)
{
	float acc_system, acc_user;
	int x_position;
	GSList *tmp;

	acc_system = acc_user = 0.0f;

	cairo_set_line_width(cr, 0);
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);

	draw_cpu_usage_background(ps, widget, cr);

	//no_cpu = SYSTEM_CPU_NO_CPUS(system_cpu);
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

		acc_system += cpu_data->system_time_percent;
		acc_user   += cpu_data->user_time_percent;

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
		layout = create_pango_layout(cr, "Sans 7");
		cairo_set_line_width(cr, 0);
		sprintf(buf, "CPU %d", cpu_data->cpu_no);

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


	x_position = draw_user_vs_system_char(cr, x_position, acc_system, acc_user);


	gtk_widget_set_size_request(widget,
			x_position,
			CPU_USAGE_MARGIN_TOP + CPU_USAGE_CHART_HEIGHT + CPU_USAGE_CPU_LABEL_HEIGHT);

	return x_position;
}


static int draw_cpu_waterfall_chart(struct ps *ps, GtkWidget *widget,
		cairo_t *cr, struct cpu_waterfall *cpu_waterfall, int x_position)
{
	unsigned int i, j, uninitialized_steps;
	int draw_cpu_auxiliary_lines;
	struct ps_color zero_color;
	int offset;
	int x_pos_pos;

	(void)ps;
	(void)widget;

	draw_cpu_auxiliary_lines = 0;

	cairo_set_antialias(cr, CAIRO_ANTIALIAS_DEFAULT);

	offset = 0;
	x_pos_pos = x_position + 100;

	uninitialized_steps = cpu_waterfall->max_elements - cpu_waterfall->ring_buffer_elements;

	if (uninitialized_steps > 0) {
		/* partial filled (we are at start phase)
		 * and draw background instead */
		cpu_waterfall_zero_color(&zero_color);
		ps_set_source_rgba(cr, &zero_color);


		for (i = 0; i < uninitialized_steps; i++) {
			int y_pos;

			y_pos = 40;
			cairo_rectangle(cr, x_pos_pos, y_pos, 10, (cpu_waterfall->no_cpu * 30));
			cairo_fill(cr);

			x_pos_pos += 10;
		}
	}


	for (i = 0; i < cpu_waterfall->ring_buffer_elements; i++) {
		struct ps_color waterfall_entry[cpu_waterfall->no_cpu];
		offset += ring_buffer_read_at(cpu_waterfall->ring_buffer,
				waterfall_entry,
				cpu_waterfall->no_cpu * sizeof(struct ps_color), offset);
		for (j = 0; j < cpu_waterfall->no_cpu; j++) {
			int y_pos;

			cairo_set_source_rgba(cr,
					      waterfall_entry[j].red,
					      waterfall_entry[j].green,
					      waterfall_entry[j].blue,
					      waterfall_entry[j].alpha);
			y_pos = 40 + (j * 30);
			cairo_rectangle(cr, x_pos_pos, y_pos, 10, 30);
			cairo_fill(cr);
		}
		x_pos_pos += 10;
	}

	if (draw_cpu_auxiliary_lines) {
		int x_start = x_position + 100;
		int x_end   = x_start + cpu_waterfall->max_elements * 10;

		cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
		cairo_set_line_width(cr, 1.0);

		for (j = 0; j < cpu_waterfall->no_cpu + 1; j++) {
			int y_pos;

			y_pos = 40 + (j * 30);

			cairo_set_source_rgba(cr, .1, .1, .1, 1.);
			cairo_move_to(cr, x_start, y_pos);
			cairo_line_to(cr, x_end, y_pos);
			cairo_stroke(cr);
		}
	}

	return x_pos_pos;
}


static void draw_interrupt_monitor_charts(struct ps *ps, GtkWidget *widget,
		cairo_t *cr,
		struct interrupt_monitor_data *imd,
		int x_position)
{
	unsigned int i, j;
	int y_position;
	PangoLayout *layout;

	(void) ps;
	(void) widget;

	x_position += CPU_USAGE_SYS_USER_USAGE_GAP;

	/* background rectangle */
	cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
	cairo_rectangle(cr, x_position,
			MARGIN_TOP,
			SYS_USER_USAGE_CHART_WIDTH + 100,
			SYS_USER_USAGE_MARGIN_TOP + SYS_USER_USAGE_CHART_HEIGHT + 10);
	cairo_fill(cr);

	layout = create_pango_layout(cr, "Sans 8");
	cairo_set_source_rgb(cr, 0.3, 0.3, 0.3);

	/* ok, we draw the background - not we can move forward */
	x_position += PADDING_LEFT;
	y_position = MARGIN_TOP + PADDING_TOP;

	//y_position = PA

	for (i = 0; i < imd->interrupt_data_array->len; i++) {
		struct interrupt_data *interrupt_data;

		interrupt_data = &g_array_index(imd->interrupt_data_array, struct interrupt_data, i);
		pango_layout_set_text(layout, interrupt_data->name, -1);
		cairo_move_to(cr, x_position, y_position);
		pango_cairo_show_layout(cr, layout);
		cairo_stroke(cr);

		for (j = 0; j < interrupt_data->irq_array->len; j++) {
			char buf[16];
			struct irq_start_current *irq_start_current;

			irq_start_current = &g_array_index(interrupt_data->irq_array,  struct irq_start_current, j);
			snprintf(buf, 16, "%ld", irq_start_current->current);
			buf[15] = '\0';
			pango_layout_set_text(layout, buf, -1);
			cairo_move_to(cr, x_position + ((j + 1) * 70), y_position);
			pango_cairo_show_layout(cr, layout);
			cairo_stroke(cr);

		}

		y_position += 10;
	}

	gtk_widget_set_size_request(widget, x_position, y_position);

	g_object_unref(layout);
}


static gboolean cpu_usage_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	int x_position;
	struct ps *ps;
	struct system_cpu *system_cpu;
	struct cpu_waterfall *cpu_waterfall;

	(void) data;

	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);
	system_cpu = g_object_get_data(G_OBJECT(widget), "system-cpu");
	assert(system_cpu);
	cpu_waterfall = g_object_get_data(G_OBJECT(widget), "cpu-waterfall");
	assert(cpu_waterfall);

	/* draw CPU bar charts */
	x_position = draw_cpu_usage_chart(ps, widget, cr, system_cpu);
	x_position = draw_cpu_waterfall_chart(ps, widget, cr, cpu_waterfall, x_position);

	return FALSE;
}


static gboolean cpu_usage_configure_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	(void)widget;
	(void)event;
	(void)data;

	return FALSE;
}


static void system_tab_timer_cpu_cb(GtkWidget *widget)
{
	int width, height;
	struct ps *ps;
	struct system_cpu *sc;
	struct cpu_waterfall *cpu_waterfall;

	if (!gtk_widget_get_visible(widget)) {
		fprintf(stderr, "not visible\n");
	}

	/* get data first */
	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);
	sc = g_object_get_data(G_OBJECT(widget), "system-cpu");
	assert(sc);
	cpu_waterfall = g_object_get_data(G_OBJECT(widget), "cpu-waterfall");
	assert(cpu_waterfall);

	system_cpu_update(ps, sc);
	cpu_waterfall_update(ps, cpu_waterfall, sc);

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	gtk_widget_queue_draw_area(widget, 0, 0, width, height);

	return;
}


static void system_tab_timer_intr_cb(GtkWidget *widget)
{
	int width, height;
	struct ps *ps;
	struct interrupt_monitor_data *interrupt_monitor_data;

	if (!gtk_widget_get_visible(widget)) {
		fprintf(stderr, "not visible\n");
	}

	/* get data first */
	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);
	interrupt_monitor_data = g_object_get_data(G_OBJECT(widget), "interrupt-monitor-data");
	assert(interrupt_monitor_data);

	interrupt_monitor_ctrl_update(ps, interrupt_monitor_data);

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	gtk_widget_queue_draw_area(widget, 0, 0, width, height);

	return;
}


/*
 * timer multiplexer into each panel within
 * the system tab to unify the timer and do not
 * trigger three different timers.
 */
static gboolean system_tab_timer_cb(GtkWidget **widget)
{
	system_tab_timer_cpu_cb(widget[SYSTEM_TAB_CPU_WIDGET]);
	system_tab_timer_intr_cb(widget[SYSTEM_TAB_INTR_WIDGET]);

	return TRUE;
}


static GtkWidget *cpu_usage_widget_new(struct ps *ps)
{
	GtkWidget *darea;
	struct system_cpu *system_cpu;
	struct cpu_waterfall *cpu_waterfall;

	darea = gtk_drawing_area_new();

	system_cpu = system_cpu_new(ps);
	if (!system_cpu)
		pr_error(ps, "Could not initilize system CPU monitor");

	cpu_waterfall = cpu_waterfall_new(ps);
	if (!cpu_waterfall)
		pr_error(ps, "Could not initilize waterfall gui");

	system_cpu_update(ps, system_cpu);

	g_signal_connect(darea, "draw", G_CALLBACK(cpu_usage_draw_cb), NULL);
	g_signal_connect(darea, "configure-event", G_CALLBACK(cpu_usage_configure_cb), NULL);

	g_object_set_data(G_OBJECT(darea), "ps", ps);
	g_object_set_data(G_OBJECT(darea), "system-cpu", system_cpu);
	g_object_set_data(G_OBJECT(darea), "cpu-waterfall", cpu_waterfall);

	return darea;
}


static void draw_interrupt_monitor_charts_bg(struct ps *ps, GtkWidget *widget, cairo_t *cr)
{
	guint width, height;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	gdk_cairo_set_source_rgba(cr, &ps->si.color[BG_COLOR_DARKER]);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);
}


static gboolean intr_usage_draw_cb(GtkWidget *widget, cairo_t *cr, gpointer data)
{
	struct ps *ps;
	struct interrupt_monitor_data *interrupt_monitor_data;

	(void)data;

	ps = g_object_get_data(G_OBJECT(widget), "ps");
	assert(ps);
	interrupt_monitor_data = g_object_get_data(G_OBJECT(widget), "interrupt-monitor-data");
	assert(interrupt_monitor_data);
	draw_interrupt_monitor_charts_bg(ps, widget, cr);
	draw_interrupt_monitor_charts(ps, widget, cr, interrupt_monitor_data, 0);

	return FALSE;
}


static gboolean intr_usage_configure_cb(GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
	(void)widget;
	(void)event;
	(void)data;

	return FALSE;
}


static GtkWidget *intr_usage_widget_new(struct ps *ps)
{
	GtkWidget *darea;
	struct interrupt_monitor_data *interrupt_monitor_data;

	darea = gtk_drawing_area_new();

	interrupt_monitor_data = interrupt_monitor_data_new(ps);
	if (!interrupt_monitor_data) {
		pr_error(ps, "Could not initilize interrupt monitor");
		gtk_widget_destroy(darea);
		return NULL;
	}

	interrupt_monitor_ctrl_update(ps, interrupt_monitor_data);

	g_signal_connect(darea, "draw", G_CALLBACK(intr_usage_draw_cb), NULL);
	g_signal_connect(darea, "configure-event", G_CALLBACK(intr_usage_configure_cb), NULL);

	g_object_set_data(G_OBJECT(darea), "ps", ps);
	g_object_set_data(G_OBJECT(darea), "interrupt-monitor-data", interrupt_monitor_data);

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

	(void) ps;

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
	static GtkWidget *widgets[SYSTEM_TAB_MAX];
	GtkWidget *vbox;
	GtkWidget *header;

	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	header = header_status_widget(ps, " CPU Information");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);
	gtk_widget_show_all(header);
	widgets[SYSTEM_TAB_CPU_WIDGET] = cpu_usage_widget_new(ps);
	gtk_box_pack_start(GTK_BOX(vbox), widgets[SYSTEM_TAB_CPU_WIDGET],
			   FALSE, TRUE, 0);
	gtk_widget_show_all(widgets[SYSTEM_TAB_CPU_WIDGET]);


	header = header_status_widget(ps, " Interrupt Information");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);
	gtk_widget_show_all(header);
	widgets[SYSTEM_TAB_INTR_WIDGET] = intr_usage_widget_new(ps);
	gtk_box_pack_start(GTK_BOX(vbox), widgets[SYSTEM_TAB_INTR_WIDGET],
			   FALSE, TRUE, 0);
	gtk_widget_show_all(widgets[SYSTEM_TAB_INTR_WIDGET]);



	header = header_status_widget(ps, " Memory and Slab Information");
	gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, TRUE, 0);
	gtk_widget_show_all(header);

	g_timeout_add(CPU_USAGE_REFRESH,(GSourceFunc)system_tab_timer_cb, widgets);

	return vbox;
}


static GtkWidget *system_notebook_tab_new(struct ps *ps)
{
        GtkWidget *frame;

	frame = system_tab_new(ps);
	gtk_container_set_border_width(GTK_CONTAINER(frame), 2);
	gtk_widget_show(frame);

	return frame;
}


GtkWidget *gui_amc_new(struct ps *ps)
{
        GtkWidget *label;
	GtkWidget *system_frame;
	GtkWidget *scroll_widget;

	ps->s.amc_notebook = gtk_notebook_new();
	gtk_notebook_popup_enable(GTK_NOTEBOOK(ps->s.amc_notebook));
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK(ps->s.amc_notebook), GTK_POS_TOP);

	scroll_widget = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_widget),
				       GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll_widget),
					    GTK_SHADOW_OUT);
	system_frame = system_notebook_tab_new(ps);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_widget), system_frame);

	label = gtk_label_new("System");
	gtk_notebook_append_page(GTK_NOTEBOOK(ps->s.amc_notebook), scroll_widget, label);
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(ps->s.amc_notebook), scroll_widget, TRUE);

	gtk_notebook_set_current_page(GTK_NOTEBOOK(ps->s.amc_notebook), 0);
	gtk_widget_set_size_request(ps->s.amc_notebook, 50, -1);

	return ps->s.amc_notebook;
}


