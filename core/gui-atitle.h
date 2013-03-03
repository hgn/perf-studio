#ifndef GUI_ATITLE_H
#define GUI_ATITLE_H

#include "perf-studio.h"

void gui_atitle_init(struct ps *ps);
void gui_atitle_set_title(struct ps *ps, const gchar *title);
void gui_atitle_set_tooltip(struct ps *ps, const gchar *tooltip);

#endif
