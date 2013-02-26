#include <string.h>

#include "perf-studio.h"
#include "event.h"
#include "shared.h"


struct event *event_new(void)
{
	return g_malloc0(sizeof(struct event));
}


void event_free(struct event *e)
{
	g_free(e);
}

void event_print(struct ps *ps, struct event *e)
{
	pr_info(ps, "event type: %d", e->type);
}
