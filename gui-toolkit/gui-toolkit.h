#ifndef GUI_TOOLKIT_H
#define GUI_TOOLKIT_H

#include "perf-studio.h"

GtkWidget *gt_stub_widget(struct ps *ps, const gchar *, guint width, guint height);
void gt_set_widget_transparent(struct ps *ps, GtkWidget *w);

#endif
