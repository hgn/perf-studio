#include "gui-toolkit.h"

PangoLayout *create_pango_layout(cairo_t *cr, const char *font_desc)
{
	PangoFontDescription *desc;
	PangoLayout *layout;

	layout = pango_cairo_create_layout(cr);
	desc = pango_font_description_from_string(font_desc);
	pango_layout_set_font_description(layout, desc);
	pango_font_description_free(desc);

	return layout;
}
