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

#endif
