#include <assert.h>

#include "gui-waterfall.h"
#include "system-info.h"
#include "shared.h"


/* How many entries should be backlogged for
 * no_cpus? Lets assume we discover 8 online CPUs
 * we define a backlog of 50 steps. Resulting in
 * 400 entries and each enrty consume sizeof(GTKColor)
 * bytes (4 byte). Resulting in 1600 bytes
 */
#define CIRC_BUFFER_MAX_STEPS 50

struct cpu_waterfall *cpu_waterfall_new(struct ps *ps)
{
	size_t req_circ_buf_mem;
	struct cpu_waterfall *cpu_waterfall;

	cpu_waterfall = g_slice_alloc0(sizeof(struct cpu_waterfall));

	cpu_waterfall->no_cpu = sysconf(_SC_NPROCESSORS_ONLN);
	if (cpu_waterfall->no_cpu < 0) {
		pr_error(ps, "Cannot determine number of CPUS online");
		goto err;
	}

	req_circ_buf_mem = cpu_waterfall->no_cpu *
			   CIRC_BUFFER_MAX_STEPS *
			   sizeof(GdkColor);
	cpu_waterfall->ring_buffer = ring_buffer_new(req_circ_buf_mem);

	return cpu_waterfall;

err:
	g_slice_free1(sizeof(*cpu_waterfall), cpu_waterfall);
	return NULL;
}


static void cpu_waterfall_calc_color(int percent, GdkColor *color)
{
	color->red   = 0;
	color->green = 0;
	color->blue  = (guint16)(65535.0 * ((float)percent / 100.0f));
	fprintf(stderr, "new color: %d\n", color->blue);
}


void cpu_waterfall_add_data(struct ps *ps, struct cpu_waterfall *cw, struct system_cpu *system_cpu)
{
	int i;
	GSList *tmp;
	GdkColor waterfall_entry[cw->no_cpu];

	(void) ps;

	assert(cw->no_cpu == SYSTEM_CPU_NO_CPUS(system_cpu));

	if (cw->ring_buffer_elements >= CIRC_BUFFER_MAX_STEPS) {
		/* remove last element */
		ring_buffer_purge(cw->ring_buffer, cw->no_cpu * sizeof(GdkColor));
		cw->ring_buffer_elements--;
	}

	i = 0;
	tmp = system_cpu->cpu_data_list;
	while (tmp) {
		int sys_user_percent;
		struct cpu_data *cpu_data;

		assert(tmp && tmp->data);
		cpu_data = tmp->data;

		sys_user_percent = cpu_data->system_time_percent +
				   cpu_data->user_time_percent;

		cpu_waterfall_calc_color(sys_user_percent, &waterfall_entry[i]);
		tmp = g_slist_next(tmp);
		i++;
	}

	ring_buffer_write(cw->ring_buffer, waterfall_entry, cw->no_cpu * sizeof(GdkColor));
	cw->ring_buffer_elements++;

}


void cpu_waterfall_free(struct cpu_waterfall *cpu_waterfall)
{
	ring_buffer_free(cpu_waterfall->ring_buffer);
	g_slice_free1(sizeof(*cpu_waterfall), cpu_waterfall);

}
