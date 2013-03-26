#ifndef EVENT_H
#define EVENT_H


#include "perf-studio.h"


enum {
	EVENT_TYPE_COUNTER = 0,
	EVENT_TYPE_SAMPLING,

	EVENT_TYPE_PERF_RECORD,

	EVENT_TYPE_MAX
};

enum {
	USERKERNELSPACE,
	USERSPACE,
	KERNELSPACE,
	HYPERVISOR,
};

struct event_counting {
	int event;
	int where;
};

struct event_sampling {
	int event;
	int where;
};

struct event {
	guint type;
	union {
		struct event_sampling sampling;
		struct event_counting counting;
	};
};


/* event basic operations */
struct event *event_new(void);
void event_free(struct event *);

/* debug and statistic functions */
void event_print(struct ps *, struct event *e);


struct events {
	GSList *event_list;
};

struct events *events_new(void);
void events_purge_all(struct events *e);


struct project_event_storage {
	gchar *filepath;
};


#endif
