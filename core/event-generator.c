#include <string.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "module-utils.h"
#include "shared.h"
#include "event.h"
#include "event-generator.h"
#include "shared.h"


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
	pr_debug(ps, "save trace file under %s", pes->filepath);

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


void event_gen_data_for_project(struct ps *ps, struct project *project, struct events *events)
{
	(void)ps;
	(void)project;
	(void)events;
}




