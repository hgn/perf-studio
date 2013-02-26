#ifndef EVENT_H
#define EVENT_H


#include "perf-studio.h"


/* event basic operations */
struct event *event_new(void);
void event_free(struct event *);

/* debug and statistic functions */
void event_print(struct ps *, struct event *e);

#endif
