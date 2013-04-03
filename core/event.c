#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "module-utils.h"
#include "shared.h"
#include "event.h"
#include "event-generator.h"
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


struct events *events_new(void)
{
	return g_malloc0(sizeof(struct events));
}


void events_add_event(struct events *events, struct event *event)
{
	assert(events);
	assert(event);

	events->event_list = g_slist_append(events->event_list, event);
}


void events_free(struct events *e)
{
	g_free(e);
}


void events_purge_all(struct events *events)
{
	GSList *tmp;
	struct event *event;

	assert(events);

	tmp = events->event_list;
	while (tmp) {
		event = tmp->data;
		assert(event);
		event_free(event);
		tmp = g_slist_next(tmp);
	}

	events_free(events);
}


int event_perf_opt(int type, char *opt, size_t opt_max)
{
	if (opt_max < 1)
		return -ENOBUFS;

	if (type >= EVENT_PERF_MAX)
		return -ERANGE;

	switch (type)
	{
	case EVENT_PERF_CYCLES:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_INSTRUCTIONS:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_CONTEXT_SWITCHES:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_TAKS_CLOCKS:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_CPU_MIGRATIONS:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_PAGE_FAULTS:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_STALLED_CYCLES_FRONTEND:
		memcpy(opt, "c", 2);
		break;
	case EVENT_PERF_STALLED_CYCLES_BACKEND:
		memcpy(opt, "c", 2);
		break;
	default:
		return -ERANGE;
	}

	return 0;
}


/* must be freed via g_char() */
gchar *events_repr(struct events *events)
{
	int ret;
	GSList *tmp_list;
	char buf[128], event_repr[8];

	assert(events);

	if (!events->event_list)
		return NULL;

	buf[0] = '\0';
	tmp_list = events->event_list;
	while (tmp_list) {
		struct event *event;
		int event_code;
		struct event_counting *event_counting;

		event = tmp_list->data;
		assert(event);

		assert(event->type == EVENT_TYPE_COUNTING);

		event_counting = &event->counting;

		event_code = event_counting->event_code;

		ret = event_perf_opt(event_code, event_repr, sizeof(event_repr));
		assert(ret == 0);

		// FIXME: handle buf overflow
		strcat(buf, event_repr);

		tmp_list = g_slist_next(tmp_list);
	}

	assert(strlen(buf) > 0);

	return strdup(buf);
}




