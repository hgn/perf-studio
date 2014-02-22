#include <errno.h>

#include "perf-studio.h"
#include "measurement-class.h"
#include "log.h"
#include "strbuf.h"

struct mc_perf_record_data {
	/* list of perf events, e.g. -e, --event */
	struct strbuf event_string;

	/* -a, --all-cpus */
	gboolean system_wide;

	/* -g, --call-graph */
	gboolean call_graph;
};




static struct mc_perf_record_data *mc_perf_record_data_alloc(void)
{
	return g_malloc0(sizeof(struct mc_perf_record_data));
}


struct mc_perf_record_data *mc_perf_record_data_create(void)
{
	struct mc_perf_record_data *mc_perf_record_data;

	mc_perf_record_data = mc_perf_record_data_alloc();

	strbuf_init(&mc_perf_record_data->event_string, 48);
	mc_perf_record_data->system_wide = FALSE;
	mc_perf_record_data->call_graph  = TRUE;

	return mc_perf_record_data;

}


void mc_perf_record_data_free(struct mc_perf_record_data *mc_perf_record_data)
{
	assert(mc_perf_record_data);

	strbuf_release(&mc_perf_record_data->event_string);

	g_free(mc_perf_record_data);
	mc_perf_record_data = NULL;
}


void mc_perf_record_data_free_recursive(struct mc_perf_record_data *mc_perf_record_data)
{
	assert(mc_perf_record_data);



	mc_perf_record_data_free(mc_perf_record_data);
}


/*
 * Add a new event (-e <event>) to perf record. Later
 * we may add additional functions to make this easier.
 * mc_perf_record_data_add_raw() is easy and powerfull so
 * it should be sufficient for the start
 *
 * This function return 0 or negative error code in the case
 * of an error
 */
int mc_perf_record_data_add_raw(struct mc_perf_record_data *mc_perf_record_data, const char *event)
{
	assert(mc_perf_record_data);

	if (!event)
		return -EINVAL;

	strbuf_addstr(&mc_perf_record_data->event_string, "-e ");
	strbuf_addstr(&mc_perf_record_data->event_string, event);

	return 0;
}


void mc_perf_record_data_callgraph_enable(struct mc_perf_record_data *mc_perf_record_data)
{
	mc_perf_record_data->call_graph = TRUE;
}


void mc_perf_record_data_callgraph_disable(struct mc_perf_record_data *mc_perf_record_data)
{
	mc_perf_record_data->call_graph = FALSE;
}


void mc_perf_record_data_system_wide_enable(struct mc_perf_record_data *mc_perf_record_data)
{
	mc_perf_record_data->system_wide = TRUE;
}


void mc_perf_record_data_system_wide_disable(struct mc_perf_record_data *mc_perf_record_data)
{
	mc_perf_record_data->system_wide = FALSE;
}


int mc_perf_record_data_check(struct mc_perf_record_data *data)
{
	if (!data)
		return -EINVAL;

	return 0;
}

