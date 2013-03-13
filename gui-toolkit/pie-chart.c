#include <assert.h>

#include "gui-toolkit.h"

/*
 * float: 4 byte + PIE_CHART_LABEL_MAX(8) = 12 byte
 * Standard cacheline: 64 byte, thus 5 entries will
 * fit into one cacheline
 */
struct pie_data_slot {
        float angle;
        char label[PIE_CHART_LABEL_MAX];
};


struct gt_pie_chart *gt_pie_chart_new(struct ps *ps)
{
        struct gt_pie_chart *gtpc;

	(void)ps;

        gtpc = g_malloc(sizeof(*gtpc));
        gtpc->pie_data_slot_array = g_array_new(FALSE, FALSE, sizeof(struct pie_data_slot));
        return gtpc;
}


void gt_pie_chart_free(struct gt_pie_chart *gt_pie_chart)
{
        assert(gt_pie_chart);
        assert(gt_pie_chart->pie_data_slot_array);

        g_array_free(gt_pie_chart->pie_data_slot_array, TRUE);
        g_free(gt_pie_chart);
}


void gt_pie_chart_set_linewidth(struct gt_pie_chart *gt_pie_chart, int line_width)
{
        gt_pie_chart->line_width = line_width;
}


void gt_pie_chart_set_offsets(struct gt_pie_chart *gt_pie_chart, int x_offset, int y_offset)
{
        gt_pie_chart->xo = x_offset;
        gt_pie_chart->yo = y_offset;
}

void gt_pie_chart_set_radius(struct gt_pie_chart *gt_pie_chart, int inner_radius, int outer_radius)
{
	gt_pie_chart->inner_radius = inner_radius;
	gt_pie_chart->outer_radius = outer_radius;
}


void gt_pie_chart_set_data(struct gt_pie_chart *gt_pie_chart, GSList *chart_data_list)
{
#if 0
        int i;
        int sum;
        float percentages[length];
        float angle[length];

        sum = 0;
        for (i = 0; i < lenght; i++) {
                sum += values[i];
        }

        for (i = 0; i < lenght; i++) {
                percentages[i] = ((float)values[i] / max) * 360.0;
                angle[i] = percentages[i] * (M_PI / 180.0);
        }
#endif
}


void gt_pie_chart_draw(struct ps *ps, struct gt_pie_chart *gtpc, cairo_t *cr)
{
#if 0
        double x_offset, y_offset;

        cairo_set_line_width (cr, 10.0);
        cairo_arc (cr, gtpc->xo, gtpc->yo, gtpc->outer_radius, angle1, angle2);
        cairo_stroke (cr);

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
#endif
}
