#ifndef EVENT_GENERATOR_H
#define EVENT_GENERATOR_H


#include "perf-studio.h"
#include "event.h"

void event_gen_data_for_project(struct ps *ps, struct project *project, struct events *events);


struct project_event_storage {
	gchar *filepath;
};


#endif /* EVENT_GENERATOR_H */
