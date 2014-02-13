#include <errno.h>

#include "perf-studio.h"
#include "measurement-class.h"
#include "log.h"

struct mc_perf_record_event *mc_perf_record_event_alloc(void)
{
	return g_malloc0(sizeof(struct mc_perf_record_event));
}


void mc_perf_record_event_free(struct mc_perf_record_event *mc_perf_record_event)
{
	assert(mc_perf_record_event);
	g_free(mc_perf_record_event);
	mc_perf_record_event = NULL;
}


struct mc_perf_record_data *mc_perf_record_data_alloc(void)
{
	return g_malloc0(sizeof(struct mc_perf_record_data));
}


void mc_perf_record_data_free(struct mc_perf_record_data *mc_perf_record_data)
{
	assert(mc_perf_record_data);

	g_free(mc_perf_record_data);
	mc_perf_record_data = NULL;
}


void mc_perf_record_data_free_recursive(struct mc_perf_record_data *mc_perf_record_data)
{
	GSList *tmp;
	struct mc_perf_record_event *mc_perf_record_event;

	tmp = mc_perf_record_data->mc_perf_record_event_list;
	while (tmp) {
		mc_perf_record_event = tmp->data;
		assert(mc_perf_record_event);

		mc_perf_record_event_free(mc_perf_record_event);

		tmp = g_slist_next(tmp);
	}

	mc_perf_record_data_free(mc_perf_record_data);
}

struct mc_element *mc_element_alloc(void)
{
	return g_malloc0(sizeof(struct mc_element));
}

void mc_element_free(struct mc_element *l)
{
	assert(l);
	g_free(l);
}

void mc_element_free_recursive(struct mc_element *l)
{
	assert(l);
	g_free(l);
}


struct mc_store *mc_store_alloc(void)
{
	return g_malloc0(sizeof(struct mc_store));
}

void mc_store_free(struct mc_store *mc_store)
{
	assert(mc_store);
	g_free(mc_store);
	mc_store = NULL;
}


/* free all memory associated with mc_store */
void mc_store_free_recursive(struct mc_store *mc_store)
{
	GSList *tmp;
	struct mc_element *mc_element;

	tmp = mc_store->mc_element_list;
	while (tmp) {
		struct mc_perf_record_data *mc_perf_record_data;

		mc_element = tmp->data;

		if (!mc_element) {
			/* a non list should not happend here */
			log_print(LOG_ERROR, "Should not happend here");
			tmp = g_slist_next(tmp);
			continue;
		}

		switch (mc_element->measurement_class) {
		case MEASUREMENT_CLASS_PERF_RECORD:
			mc_perf_record_data = (struct mc_perf_record_data *)mc_element->mc_element_data;
			mc_perf_record_data_free_recursive(mc_perf_record_data);
			break;
		default:
			log_print(LOG_WARNING, "No free function implemented");
		};

		tmp = g_slist_next(tmp);
	}

	mc_store_free(mc_store);

	return;
}

static gboolean mc_perf_record_data_sanity_check(struct mc_perf_record_data *data)
{
	if (!data)
		return FALSE;

	if (!data->mc_perf_record_event_list) {
		log_print(LOG_ERROR, "Registered perf record data specifies no events");
		return FALSE;
	}

	return TRUE;
}


static gboolean check_mc_element_sanity(struct project *project, struct mc_element *mc_element)
{
	gboolean ret = TRUE;


	(void) project;

	if (mc_element->measurement_class >= MEASUREMENT_CLASS_MAX) {
		/* if one element is not sane we return with an error,
		 * another solution is to drop the wrong element. But
		 * this may lead to worse code! Why should we accept
		 * bad code here? This makes no sense.
		 */
		log_print(LOG_ERROR, "unknown measurement class: %d",
				mc_element->measurement_class);
		return FALSE;
	}

	switch (mc_element->measurement_class) {
	case MEASUREMENT_CLASS_PERF_RECORD:
		return mc_perf_record_data_sanity_check((struct mc_perf_record_data *)mc_element->mc_element_data);
		break;
	default:
		log_print(LOG_WARNING, "No sanity check implemented");
	};

	return ret;
}


static gboolean check_mc_store_sanity(struct project *project,
		struct mc_store *mc_store)
{
	GSList *tmp;
	gboolean gret;
	struct mc_element *mc_element;

	tmp = mc_store->mc_element_list;
	while (tmp) {
		mc_element = tmp->data;

		if (!mc_element) {
			log_print(LOG_ERROR, "XXX");
			return FALSE;
		}

		gret = check_mc_element_sanity(project, mc_element);
		if (gret == FALSE) {
			return FALSE;
		}

		/*
		 * FIXME:
		 * here we should search for duplicate measurement_class
		 * entries in the mc_store (e.g. by accounting measurement_classes
		 * in a bitmask. Just to make sure there is no duplicate class,
		 * e.g. MEASUREMENT_CLASS_RAW_STDOUT_STDERR_CAPTURE
		 */

		tmp = g_slist_next(tmp);
	}
	return TRUE;
}


/* FIXME
 * projects should contain a reference to */

/* Called from module when module is activated to register
 * module specific measurement classes.
 *
 */
int project_register_mc_store(struct project *project, struct mc_store *mc_store)
{
	gboolean bret;

	assert(project);
	assert(project->ps);
	assert(mc_store);
	assert(mc_store->owner);

	/* first we check if this mc_store is already registered.
	 * Only one mc_store can be registered at one time */
	//events->event_list = g_slist_append(events->event_list, event);
	if (project->mc_store) {
		log_print(LOG_ERROR, "");
		return -EEXIST;
	}

	/* now we iterate over store and check sanity of data */
	bret = check_mc_store_sanity(project, mc_store);
	if (bret == FALSE) {
		log_print(LOG_ERROR, "mc store data currupt");
		return -EINVAL;
	}

	/* save pointer */
	project->mc_store = mc_store;
	log_print(LOG_INFO, "registered measurement class store at project");

	return 0;
}


struct mc_store *project_unregister_mc_store(struct project *project)
{
	struct mc_store *store;

	if (!project->mc_store) {
		log_print(LOG_ERROR, "Cannot unregister because no measurement store registered");
		return NULL;
	}

	store = project->mc_store;
	project->mc_store = NULL;

	log_print(LOG_INFO, "Measurement unregistered at project");

	return store;
}


