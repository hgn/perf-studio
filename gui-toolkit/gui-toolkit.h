#ifndef GUI_TOOLKIT_H
#define GUI_TOOLKIT_H

#include "perf-studio.h"


GtkWidget *gt_stub_widget(struct ps *ps, const gchar *, guint width, guint height);
void gt_set_widget_transparent(struct ps *ps, GtkWidget *w);

/* font.c */
PangoLayout *create_pango_layout(cairo_t *cr, const char *font_desc);

/* canvas.c */
void gui_err_dialog(struct ps *ps, const gchar *format, ...);
void gui_msg_dialog(struct ps *ps, const gchar *format, ...);


/* pie-chart.h */
#define PIE_CHART_LABEL_MAX 32

enum {
	GT_PIE_CHART_MODE_PIE,
	GT_PIE_CHART_MODE_DONUT
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
        struct ps_color fg_color;
	int mode;
};

#define DEG_TO_RAD(x) (((float)x) * M_PI / 180.0)

struct kv_list;

void gt_pie_chart_set_data(struct gt_pie_chart *gt_pie_chart, struct kv_list *);
struct gt_pie_chart *gt_pie_chart_new(void);
void gt_pie_chart_free(struct gt_pie_chart *gt_pie_chart);
void gt_pie_chart_draw(struct ps *ps, GtkWidget *widget, cairo_t *cr, struct gt_pie_chart *gtpc);
void gt_pie_chart_set_linewidth(struct gt_pie_chart *gt_pie_chart, int linewidth);
void gt_pie_chart_set_offsets(struct gt_pie_chart *gt_pie_chart, int x_offset, int y_offset);
void gt_pie_chart_set_radius(struct gt_pie_chart *gt_pie_chart, int inner_radius, int outer_radius);
void gt_pie_chart_set_fg_color(struct gt_pie_chart *gt_pie_chart, const struct ps_color *);


enum {
	ICON_CLOSE,

	ICON_MAX
};


GtkWidget *ps_icon(struct ps *ps, int icon_id);

#endif
