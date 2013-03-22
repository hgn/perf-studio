#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include "perf-studio.h"
#include "shared.h"
#include "disect-perf.h"
#include "event.h"


/* if the function return != something goes wrong. The caller MUST
 * instantly free the disct data. The callback is will never called!
 * If the function return 0 then everything went fine. If the disector
 * parsed the data the callback is called and the data is valid.
 * The caller must call disect_free() to free the data (which in turn
 * calls disctor specific free routins
 */
int disect_async(struct ps *ps, struct disect *disect, disect_async_cb cb)
{
	assert(ps);
	assert(disect);
	assert(cb);

	switch (disect->type) {
	case EVENT_TYPE_PERF_RECORD:
		return disect_async_perf_record(ps, disect, cb);
		break;
	default:
		pr_error(ps, "no disector for this datum");
		return -EINVAL;
	};

	return -EINVAL;
}

struct disect *disect_new(void)
{
	return g_malloc0(sizeof(struct disect));
}

void disect_free(struct ps *ps, struct disect *disect)
{
	switch (disect->type) {
		case EVENT_TYPE_PERF_RECORD:
			disect_free_perf_record(ps, disect);
			break;
		default:
			assert(0);
			break;
	}

	g_free(disect);
}
