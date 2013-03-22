#include <string.h>
#include <assert.h>

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



struct events *events_new(void)
{
	return g_malloc0(sizeof(struct events));
}

void events_add_event(struct events *events, struct event *event)
{
	events->event_list = g_slist_append(events->event_list, event);
}


void events_free(struct events *e)
{
	g_free(e);
}


void events_purge_all(struct events *e)
{
	assert(e);

	// FIXME iterate over all events and call event_free();

	events_free(e);
}


/* must be freed via g_char() */
const gchar *events_repr(struct events *events)
{
	return "";
}


/* return a filename path to save a perf data file
 * for the events specified by the second argument.
 * THe events are registered in the conf file and can
 * later be referenced by perf-studio.
 *
 * The returned path must be freed with g_free().
 *
 * XXX: if the command failes (e.g. perf failure).
 * The data is still registered in the conf file. But
 * the conf file is probably empty or even not available.
 * This
 */
struct project_event_storage *project_event_storage_new(struct ps *ps,
							struct events *events)
{
	gchar *events_repr_str;
	struct project_event_storage *pes;

	assert(ps);
	assert(ps->project);
	assert(ps->project->project_db_path);

	pes = g_malloc0(sizeof(*pes));

	events_repr_str = events_repr(events);

	pes->filepath = g_build_filename(ps->project->project_db_path,
					 events_repr_str, NULL);

	g_free(events_repr_str);

	return pes;
}


void project_event_storage_free(struct project_event_storage *pes)
{
	assert(pes);

	if (pes->filepath)
		g_free(pes->filepath);

	g_free(pes);
}

/* write new file into conf file to reflect new data */
void project_event_storage_finalize(struct project_event_storage *pes)
{
	assert(pes);
}

