#include <assert.h>

#include "perf-studio.h"
#include "shared.h"

struct project *project_new(void);
void project_free(struct project *e);
void project_purge_all(struct ps *ps);
void project_deactivate(struct ps *ps);


static inline void project_activate(struct ps *ps, struct project *project)
{
	/* if a new project is activated, the previous
	 * project must be deactived first (unload)
	 */
	assert(ps->project == 0);

	ps->project = project;
}

static inline void project_add(struct ps *ps, struct project *project)
{
	ps->project_list = g_slist_append(ps->project_list, project);
}


static inline guint project_loaded(struct ps *ps)
{
	return g_slist_length(ps->project_list);
}



