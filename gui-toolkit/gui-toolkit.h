#ifndef GUI_TOOLKIT_H
#define GUI_TOOLKIT_H

#include "perf-studio.h"


/*
 * Cairo use double, we just use a reduced
 * set of floats to be cacheline friendly
 * The high resolution is not required here
 */
struct ps_color {
	float red;
	float green;
	float blue;
	float alpha;
};

#define Hex8ToFloat(x) ((float)x / 0xff)
#define Hex16ToFloat(x) ((float)x / 0xffff)

GtkWidget *gt_stub_widget(struct ps *ps, const gchar *, guint width, guint height);
void gt_set_widget_transparent(struct ps *ps, GtkWidget *w);

/* font.c */
PangoLayout *create_pango_layout(cairo_t *cr, const char *font_desc);

/* canvas.c */
void gui_err_dialog(struct ps *ps, const gchar *format, ...);


/* pie-chart.h */
#define PIE_CHART_LABEL_MAX 8

struct chart_data {
	unsigned long value;
	char *label;
};

struct gt_pie_chart {
        GArray *pie_data_slot_array;

        /* starting offsets, relativ to cairo context */
        int xo;
        int yo;

        /* number of pixel of inner radios */
        int inner_radius;
        int outer_radius;
        int line_width;
        struct ps_color *bg_color;
        struct ps_color *fg_color;
};

void gt_pie_chart_set_data(struct gt_pie_chart *gt_pie_chart, GSList *chart_data_list);
struct gt_pie_chart *gt_pie_chart_new(struct ps *ps);
void gt_pie_chart_free(struct gt_pie_chart *gt_pie_chart);
void gt_pie_chart_draw(struct ps *ps, struct gt_pie_chart *gt_pie_chart, cairo_t *cr);
void gt_pie_chart_set_linewidth(struct gt_pie_chart *gt_pie_chart, int linewidth);
void gt_pie_chart_set_offsets(struct gt_pie_chart *gt_pie_chart, int x_offset, int y_offset);
void gt_pie_chart_set_radius(struct gt_pie_chart *gt_pie_chart, int inner_radius, int outer_radius);

#endif
