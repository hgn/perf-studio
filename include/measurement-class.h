#ifndef MEASUREMENT_CLASS_H
#define MEASUREMENT_CLASS_H

#include <stdio.h>
#include <assert.h>


#include "perf-studio.h"

#include <glib.h>


enum mc_type {
	MEASUREMENT_CLASS_RAW,
	MEASUREMENT_CLASS_RAW_STDOUT_STDERR_CAPTURE,
	MEASUREMENT_CLASS_TIME_MEASUREMENT,
	MEASUREMENT_CLASS_PERF_RECORD,

	MEASUREMENT_CLASS_MAX
};

struct mc_perf_record_event {
	int trash;
};



struct mc_perf_record_data {
	/* list of perf events, e.g. -e, --event */
	GSList *mc_perf_record_event_list;

	/* -a, --all-cpus */
	gboolean system_wide;

	/* -g, --call-graph */
	gboolean call_graph;
};



struct mc_element {
	/* one of MEASUREMENT_CLASS_* */
	unsigned int measurement_class;
	void *mc_element_data;
};


struct mc_store {
	GSList *mc_element_list;

	/* mc_stores belong to an module. To get
	 * the back reference modules must set itself.
	 * The owner is used - among other things - to
	 * know the owner when the mc_store is deregistered
	 * at a active project
	 */
	struct module *owner;
};


static inline void mc_store_set_owner(struct mc_store *s, struct module *m)
{
	assert(s);
	assert(m);

	s->owner = m;
}


struct mc_store *mc_store_alloc(void);
void mc_store_free(struct mc_store *);
void mc_store_free_recursive(struct mc_store *mc_store);
int mc_store_add(struct mc_store *mc_store, enum mc_type mc_type, void *mc_data) WARN_UNUSED_RESULT;


struct mc_element *mc_element_alloc(void);
void mc_element_free_recursive(struct mc_element *l);
void mc_element_free(struct mc_element *l);

struct mc_perf_record_event *mc_perf_record_event_alloc(void);
void mc_perf_record_event_free(struct mc_perf_record_event *mc_perf_record_event);

struct mc_perf_record_data *mc_perf_record_data_alloc(void);
void mc_perf_record_data_free(struct mc_perf_record_data *mc_perf_record_data);
void mc_perf_record_data_free_recursive(struct mc_perf_record_data *mc_perf_record_data);

int project_register_mc_store(struct project *project, struct mc_store *mc_store);
struct mc_store *project_unregister_mc_store(struct project *project);


#endif /* MEASUREMENT_CLASS_H */
