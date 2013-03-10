#ifndef GUI_HELP_D
#define GUI_HELP_D

#include "perf-studio.h"

enum {
	GUI_HELP_PAGE_OVERVIEW,
	GUI_HELP_PAGE_MISC
};

void gui_help_overview_window(GtkWidget *widget, struct ps *ps);

#endif /* GUI_HELP_D */
