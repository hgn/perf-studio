#include "disect-perf.h"


int disect_async_perf_record(struct ps *ps, struct disect *disect, disect_async_cb cb)
{
	(void) ps;
	(void) disect;
	(void) cb;

	/* call g_idle_add and call a static function. Call the original
	 * registered function (cb) if the processing is done and return false
	 * to unregister from next call.
	 * g_idle_add_full() should be taken because we can play with the
	 * priority a little bit. G_PRIORITY_HIGH_IDLE shoud be taken as the
	 * priority.
	 */

	return 0;
}

void disect_free_perf_record(struct ps *ps, struct disect *disect)
{
	(void) ps;
	(void) disect;
}
