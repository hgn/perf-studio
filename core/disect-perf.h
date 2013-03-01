#ifndef DISECT_PERF
#define	DISECT_PERF

#include "perf-studio.h"

int disect_async_perf_record(struct ps *ps, struct disect *disect, disect_async_cb);
void disect_free_perf_record(struct ps *ps, struct disect *disect);

#endif
