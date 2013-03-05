#include "gui-toolkit.h"

PangoLayout *create_pango_layout(cairo_t *cr)
{
	PangoFontDescription *desc;
	PangoLayout *layout;

	layout = pango_cairo_create_layout(cr);
	desc = pango_font_description_from_string("Sans 7");
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	return layout;
}
