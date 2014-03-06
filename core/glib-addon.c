#include "glib-addon.h"


gchar **strv_str_add(gchar **v, gchar *str)
{
      gint i = 0;
      gchar **retval;

      if (!v)
	      return NULL;

      if (!str)
	      return g_strdupv(v);

      while (v[i])
	      i++;

      retval = g_new(gchar *, i + 2);

      i = 0;
      while (v[i]) {
	      retval[i] = g_strdup(v[i]);
	      i++;
      }
      retval[i]     = g_strdup(str);
      retval[i + 1] = NULL;

      return retval;
}


gchar **strv_merge(gchar **a, gchar **b)
{
      gint i = 0, j = 0;
      gchar **retval;

      if (!a)
	      return NULL;

      if (!b)
	      return g_strdupv(a);

      while (a[i])
	      i++;
      while (b[j])
	      i++;

      retval = g_new(gchar *, i + 1);

      i = 0;
      while (a[i]) {
	      retval[i] = g_strdup(a[i]);
	      i++;
      }
      while (b[i]) {
	      retval[i] = g_strdup(b[i]);
	      i++;
      }
      retval[i] = NULL;

      return retval;
}

