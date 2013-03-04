/* area module content */
#include "gui-amc.h"
#include "gui-toolkit.h"

GtkWidget *gui_amc_new(struct ps *ps)
{
	return gt_stub_widget(ps, "AMC", 100, 100);
}
