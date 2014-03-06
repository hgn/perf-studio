#ifndef GLIB_ADDON_H
#define GLIB_ADDON_H

#include <glib.h>
#include <glib/gprintf.h>

gchar **strv_str_add(gchar **v, gchar *str);
gchar **strv_merge(gchar **a, gchar **b);


#endif /* GLIB_ADDON_H */
