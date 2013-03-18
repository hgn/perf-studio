#include <string.h>
#include <math.h>
#include <assert.h>

#include "gui-toolkit.h"
#include "kv-list.h"

#define DEFAULT_LINE_WIDTH 1
#define DEFAULT_WIDGET_WIDTH -1
#define DEFAULT_WIDGET_HEIGHT 100

#define DEFAULT_LEFT_CIRC_PADDING 10

#define DEFAULT_LABEL_FONT "sans 8"
#define PIE_CHART_TO_LABEL_PADDING 50


/* http://html-color-codes.com/ */
static const struct ps_color bg_fill_colors[] = {
	/* #336699 */
	[0] = { .red   = Hex8ToFloat(0x33),
		.green = Hex8ToFloat(0x66),
		.blue  = Hex8ToFloat(0x99),
		.alpha = 1.0
	},
	[1] = { .red   = Hex8ToFloat(0x33),
		.green = Hex8ToFloat(0x88),
		.blue  = Hex8ToFloat(0x99),
		.alpha = 1.0
	},
	[2] = { .red   = Hex8ToFloat(0x33),
		.green = Hex8ToFloat(0x99),
		.blue  = Hex8ToFloat(0x99),
		.alpha = 1.0
	},
	[3] = { .red   = Hex8ToFloat(0x33),
		.green = Hex8ToFloat(0xbb),
		.blue  = Hex8ToFloat(0x99),
		.alpha = 1.0
	}
};

struct pie_data_slot {
        float angle;
        char label[PIE_CHART_LABEL_MAX];
};


struct gt_pie_chart *gt_pie_chart_new(void)
{
        struct gt_pie_chart *gtpc;

        gtpc = g_malloc(sizeof(*gtpc));
        gtpc->pie_data_slot_array = g_array_new(FALSE, FALSE,
						sizeof(struct pie_data_slot));
	gt_pie_chart_set_linewidth(gtpc, DEFAULT_LINE_WIDTH);

        return gtpc;
}


void gt_pie_chart_free(struct gt_pie_chart *gt_pie_chart)
{
        assert(gt_pie_chart);
        assert(gt_pie_chart->pie_data_slot_array);

        g_array_free(gt_pie_chart->pie_data_slot_array, TRUE);
        g_free(gt_pie_chart);
}


void gt_pie_chart_set_linewidth(struct gt_pie_chart *gt_pie_chart,
				int line_width)
{
        gt_pie_chart->line_width = line_width;
}


void gt_pie_chart_set_offsets(struct gt_pie_chart *gt_pie_chart,
			      int x_offset, int y_offset)
{
        gt_pie_chart->xo = x_offset;
        gt_pie_chart->yo = y_offset;
}

void gt_pie_chart_set_radius(struct gt_pie_chart *gt_pie_chart,
			     int inner_radius, int outer_radius)
{
	gt_pie_chart->inner_radius = inner_radius;
	gt_pie_chart->outer_radius = outer_radius;
}


void gt_pie_chart_set_data(struct gt_pie_chart *gt_pie_chart,
			   struct kv_list *chart_data_list)
{
	int elements;
	unsigned int j;
	float sum;
	float *percentages, *angles;
	double deg;
	GSList *tmp;

	elements = 0;
	sum = 0.0;
	tmp = KV_LIST_HEAD(chart_data_list);
	while (tmp) {
		struct kv_list_entry *entry;
		entry = tmp->data;
		assert(entry);

		sum += GPOINTER_TO_INT(entry->key);

		elements++;
		tmp = g_slist_next(tmp);
	}

	percentages = g_malloc(sizeof(float) * elements);
	angles      = g_malloc(sizeof(float) * elements);

	j = 0;
	deg = 0.0;
	tmp = KV_LIST_HEAD(chart_data_list);
	while (tmp) {
		struct pie_data_slot *pie_data_slot;
		struct kv_list_entry *entry;

		entry = tmp->data;
		assert(entry);

                percentages[j] = ((float)GPOINTER_TO_INT(entry->key) / sum) * 360.0;
                angles[j] = DEG_TO_RAD(percentages[j]);
		deg += percentages[j];

		if (gt_pie_chart->pie_data_slot_array->len <= j)
			g_array_set_size(gt_pie_chart->pie_data_slot_array, j + 1);

		pie_data_slot = &g_array_index(gt_pie_chart->pie_data_slot_array,
					       struct pie_data_slot, j);
		pie_data_slot->angle = angles[j];
		memcpy(pie_data_slot->label, entry->value, sizeof(pie_data_slot->label));
		pie_data_slot->label[sizeof(pie_data_slot->label) - 1] = '\0';

		tmp = g_slist_next(tmp);
		j++;
	}

	/*
	 * FIXME: should it possible that the array must be reduced to
	 * chart_data_list length? This implementation assume that the
	 * chart_data_list is never reduced during runtime! I.e. a four
	 * segment pie chart will always have four segments. This may change
	 * in future because some pie charts user may have other requirements,
	 * but this is up to the future.
	 */

	g_free(percentages);
	g_free(angles);
}


static void draw_widget_background(struct ps *ps, cairo_t *cr,
					  int width, int height)
{
	gdk_cairo_set_source_rgba(cr, &ps->si.color[BG_COLOR_DARKER]);
	cairo_rectangle(cr, 0, 0, width, height);
	cairo_fill(cr);
}


static inline void draw_labels(cairo_t *cr, const char *text, int x, int y)
{
	PangoLayout *layout;

	cairo_save(cr);

	cairo_set_source_rgb(cr, 0.5, 0.5, 0.5);
	layout = create_pango_layout(cr, DEFAULT_LABEL_FONT);
	pango_layout_set_text(layout, text, -1);
	cairo_move_to(cr, x, y);
	pango_cairo_show_layout(cr, layout);

	cairo_stroke(cr);

	g_object_unref(layout);

	cairo_restore(cr);
}


void gt_pie_chart_draw(struct ps *ps, GtkWidget *widget, cairo_t *cr,
		       struct gt_pie_chart *gtpc)
{
	guint width, height;
	unsigned int i;
	double xc;
	double yc;
	double radius;
	double angle_start = DEG_TO_RAD(0.0);
	const struct ps_color *bg_color;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

	draw_widget_background(ps, cr, width, height);

	cairo_set_line_width(cr, gtpc->line_width);

	yc = height / 2.0;
	radius = yc - (2 * gtpc->line_width);
	xc = radius + DEFAULT_LEFT_CIRC_PADDING;

	for (i = 0; i < gtpc->pie_data_slot_array->len; i++) {
		struct pie_data_slot *pie_data_slot;

		pie_data_slot = &g_array_index(gtpc->pie_data_slot_array,
					       struct pie_data_slot, i);
		assert(angle_start + pie_data_slot->angle <= DEG_TO_RAD(360.1));

		cairo_move_to(cr, xc, yc);
		cairo_arc(cr, xc, yc, radius, angle_start, pie_data_slot->angle);
		cairo_line_to(cr, xc, yc);

		bg_color = &bg_fill_colors[i % ARRAY_SIZE(bg_fill_colors)];
		cairo_set_source_rgba(cr,
				      bg_color->red,
				      bg_color->green,
				      bg_color->blue,
				      bg_color->alpha);
		cairo_fill_preserve(cr);

		cairo_set_source_rgba(cr,
				      gtpc->fg_color.red,
				      gtpc->fg_color.green,
				      gtpc->fg_color.blue,
				      gtpc->fg_color.alpha);
		cairo_stroke(cr);

		draw_labels(cr,
			    pie_data_slot->label,
			    radius * 2 + PIE_CHART_TO_LABEL_PADDING + DEFAULT_LEFT_CIRC_PADDING,
			    10 + (i * 20));

		angle_start += pie_data_slot->angle;
	}
}


void gt_pie_chart_set_fg_color(struct gt_pie_chart *gtpc, const struct ps_color *c)
{
	memcpy(&gtpc->fg_color, c, sizeof(gtpc->fg_color));
}
