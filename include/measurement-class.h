#ifndef MEASUREMENT_CLASS_H
#define MEASUREMENT_CLASS_H

#include <stdio.h>
#include <assert.h>


#include "perf-studio.h"

#include <glib.h>


enum {
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
	 */
	struct module *owner;
};




#endif /* MEASUREMENT_CLASS_H */
