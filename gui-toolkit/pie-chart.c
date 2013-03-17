#include <assert.h>
#include <math.h>

#include "gui-toolkit.h"
#include "kv-list.h"

/*
 * float: 4 byte + PIE_CHART_LABEL_MAX(8) = 12 byte
 * Standard cacheline: 64 byte, thus 5 entries will
 * fit into one cacheline
 */
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
	int elements, i, j;
	long sum, max;
	float *percentages;
	float *angles;
	GSList *tmp;

	sum = elements = max = 0;
	tmp = KV_LIST_HEAD(chart_data_list);
	while (tmp) {
		struct kv_list_entry *entry;
		entry = tmp->data;
		assert(entry);

		sum += GPOINTER_TO_INT(entry->key);
		max = max(GPOINTER_TO_INT(entry->key), max);

		fprintf(stderr, "sum: %d\n", GPOINTER_TO_INT(entry->key));

		elements++;
		tmp = g_slist_next(tmp);
	}

	percentages = g_malloc(sizeof(float) * elements);
	angles      = g_malloc(sizeof(float) * elements);

	j = 0;
	tmp = KV_LIST_HEAD(chart_data_list);
	while (tmp) {
		struct pie_data_slot *pie_data_slot;
		struct kv_list_entry *entry;

		entry = tmp->data;
		assert(entry);

                percentages[i] = ((float)GPOINTER_TO_INT(entry->key) / sum) * 360.0;
                angles[i] = percentages[i] * (M_PI / 180.0);


		if (gt_pie_chart->pie_data_slot_array->len <= j)
			g_array_set_size(gt_pie_chart->pie_data_slot_array, j + 1);

		pie_data_slot = &g_array_index(gt_pie_chart->pie_data_slot_array,
					       struct pie_data_slot, j);
		pie_data_slot->angle = angles[i];
		memcpy(pie_data_slot->label, entry->value, sizeof(pie_data_slot->label));
		pie_data_slot->label[sizeof(pie_data_slot->label) - 1] = '\0';

		fprintf(stderr, "angle: %f %s\n",  angles[i], pie_data_slot->label);

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


void gt_pie_chart_draw(struct ps *ps, GtkWidget *widget, cairo_t *cr, struct gt_pie_chart *gtpc)
{
	guint width, height;
	GdkRGBA color;
        double x_offset, y_offset;
	double start_deg;

	width = gtk_widget_get_allocated_width(widget);
	height = gtk_widget_get_allocated_height(widget);

        cairo_set_line_width (cr, 1.0);

        cairo_set_source_rgba (cr, 1, 0.2, 0.2, 1.);

	double xc = 100.0;
	double yc = 50.0;
	double radius = 20.0;
	double angle1 = DEG_TO_RAD(0.0);
	double angle2 = DEG_TO_RAD(45.0);

	// https://code.google.com/p/homebank-maemo/source/browse/branches/4.2.1-maemo5/src/gtkchart.c#1481

	fprintf(stderr, "angle: %f\n", angle2);

	cairo_set_line_width (cr, 10.0);
	cairo_arc(cr, xc, yc, radius, angle1, angle2);
	cairo_fill(cr);

	return;

	/* draw helping lines */
	cairo_set_source_rgba (cr, 1, 0.2, 0.2, 0.6);
	cairo_set_line_width (cr, 6.0);

	cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
	cairo_fill (cr);

	cairo_arc (cr, xc, yc, radius, angle1, angle1);
	cairo_line_to (cr, xc, yc);
	cairo_arc (cr, xc, yc, radius, angle2, angle2);
	cairo_line_to (cr, xc, yc);
	cairo_stroke (cr);

#if 0

	start_deg = 0.0;

	cairo_arc(cr, width / 2.0, height / 2.0, min(width, height) / 2.0, start_deg, 2 * M_PI - 0.4);
	cairo_stroke (cr);

        //cairo_set_source_rgba (cr, .2, 1.0, 0.2, 1.);
	//cairo_arc(cr, width / 2.0, height / 2.0, min(width, height) / 2.0, start_deg, .04);
	//cairo_stroke(cr);

	return

        cairo_set_line_width (cr, 6.0);
        cairo_set_line_width (cr, 10.0);
        cairo_arc (cr, gtpc->xo, gtpc->yo, gtpc->outer_radius, 0.0, .4);
        cairo_stroke (cr);

        /* draw helping lines */

        //cairo_arc (cr, xc, yc, 10.0, 0, 2*M_PI);
        //cairo_fill (cr);

        //cairo_arc (cr, xc, yc, radius, angle1, angle1);
        //cairo_line_to (cr, xc, yc);
        //cairo_arc (cr, xc, yc, radius, angle2, angle2);
        //cairo_line_to (cr, xc, yc);
        //cairo_stroke (cr);
#endif
}
