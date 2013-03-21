#include <assert.h>

#include "perf-studio.h"
#include "shared.h"

struct project *project_new(void);
void project_free(struct project *e);
void project_purge_all(struct ps *ps);
void project_deactivate(struct ps *ps);
void project_show(struct ps *ps, struct project *p);
int project_load_by_id(struct ps *ps, const char *id);


static inline void project_add(struct ps *ps, struct project *project)
{
	ps->project_list = g_slist_append(ps->project_list, project);
}


static inline guint project_loaded(struct ps *ps)
{
	return g_slist_length(ps->project_list);
}



