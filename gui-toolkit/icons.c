#include <string.h>
#include <math.h>
#include <assert.h>

#include "gui-toolkit.h"


static const char *icon_path(struct ps *ps)
{
	return ps->si.buttondir;
}


static const char *icon_name(int i)
{
	switch (i) {
	case ICON_CLOSE:
		return "overlay-closedowntransp9.png";
		break;
	default:
		return "overlay-closedowntransp9.png";
		break;
	}

	return "overlay-closedowntransp9.png";
}


GtkWidget *ps_icon(struct ps *ps, int icon_id)
{
	char *path;
	GtkWidget *image;

	path = g_build_filename(icon_path(ps), icon_name(icon_id), NULL);
	image = gtk_image_new_from_file(path);
	g_free(path);
	return image;
}
