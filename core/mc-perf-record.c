#include <stdlib.h>
#include <errno.h>

#include "perf-studio.h"
#include "mc.h"
#include "log.h"
#include "strbuf.h"
#include "file-utils.h"

struct mc_perf_record_data {
	/* string of perf events, e.g. -e, --event */
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


gchar **mc_perf_record_data_exec_cmd(struct ps *ps,
		struct mc_perf_record_data *mc_perf_record_data)
{
	const char *cmd;
	gchar *full_cmd_path;
	gchar **ret;
	struct strbuf strbuf;

	assert(ps);
	assert(ps->conf.common.perf_path);
	assert(mc_perf_record_data);

	strbuf_init(&strbuf, 256);
	strbuf_addf(&strbuf, "%s record ", ps->conf.common.perf_path);

	strbuf_addf(&strbuf, " -f %s/perf.data ", ps->active_project->project_db_path);

	if (mc_perf_record_data->system_wide)
		strbuf_addf(&strbuf, "--all-cpus ");

	if (mc_perf_record_data->call_graph)
		strbuf_addf(&strbuf, "--call-graph ");

	/* now add the executable */
	cmd = "../perf-cases/cache-miss/cache-miss";
	full_cmd_path = file_utils_find_exec(getenv("PATH"), cmd);
	if (!full_cmd_path) {
		log_print(LOG_CRITICAL, "Could not find the executable: %s", cmd);
		ret = NULL;
		goto out;
	}

	strbuf_addf(&strbuf, "%s", full_cmd_path);
	g_free(full_cmd_path);

	ret = g_strsplit(strbuf.buf, " ", 48);
	if (!ret) {
		log_print(LOG_ERROR, "Cannot construct perf command string");
		ret = NULL;
		goto out;
	}

out:
	strbuf_release(&strbuf);

	return ret;
}


int mc_perf_record_data_prepare_results(struct ps *ps, struct mc_element *mc_element)
{
	assert(ps);
	assert(mc_element);

	mc_element->data = (void *)0xffffffff;

	return 0;
}
