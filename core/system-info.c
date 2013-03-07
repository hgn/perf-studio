#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "system-info.h"
#include "shared.h"

struct system_cpu *system_cpu_new(struct ps *ps)
{
	int ret;
	long no_cpus, i;
	struct system_cpu *system_cpu;

	system_cpu = g_slice_alloc0(sizeof(*system_cpu));

	ret = clock_gettime(CLOCK_REALTIME, &system_cpu->last_checkpointed);
	if (ret != 0)
		err_msg_die(ps, EXIT_FAILURE, "cannot get system time!");

	system_cpu->clock_tick = sysconf(_SC_CLK_TCK);
	if (system_cpu->clock_tick < 0)
		err_msg_die(ps, EXIT_FAILURE, "Cannot get SC_CLK_TCK - needs fix!");

	no_cpus = sysconf(_SC_NPROCESSORS_ONLN);
	if (no_cpus < 1) {
		err_msg_die(ps, EXIT_FAILURE, "Cannot determine number of CPUs");
	}

	for (i = 0; i < no_cpus; i++) {
		struct cpu_data *cpu_data;

		cpu_data = g_malloc0(sizeof(*cpu_data));
		cpu_data->cpu_no = i;
		system_cpu->cpu_data_list = g_slist_append(system_cpu->cpu_data_list, cpu_data);
	}

	return system_cpu;
}


void system_cpu_checkpoint(struct ps *ps, struct system_cpu *system_cpu)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	long unsigned it, kt, ut;
	long int cpu;
	long unsigned int user, system, idle;
	GSList *tmp;

	/* FIXME: do not open the file every time,
	 * just seek(0) and close at system_cpu_free()
	 * time
	 */
	fp = fopen("/proc/stat", "r");
	if (fp == NULL) {
		err_msg_die(ps, EXIT_FAILURE, "Cannot open /proc/stat - urrghl!");
	}


	tmp = system_cpu->cpu_data_list;
	while ((read = getline(&line, &len, fp)) != -1) {
		line[read - 1] = '\0';

		if (read < 5)
			continue;

		if (strncmp(line, "cpu", 3))
			continue;

		if (!isdigit(line[3]))
			continue;

		if (sscanf (line, "cpu%lu %lu %*d %lu %lu", &cpu, &user, &system, &idle) == 4) {
			struct cpu_data *cpu_data;
			assert(tmp && tmp->data);
			cpu_data = tmp->data;
			//assert(cpu_data->cpu_no == cpu);
			if (cpu != cpu_data->cpu_no) {
				/*
				 * ok, one or more CPU seems offline!
				 * There is gap between the previous cpu and
				 * the currently parsed number here in
				 * /proc/stat. We mark this CPU as offline.
				 */
				do {
					pr_info(ps, "CPU core %d disabled by system\n",
						cpu_data->cpu_no);
					/* fast forward to the current CPU and mark
					   all offline CPUs as offline */
					tmp = g_slist_next(tmp);
					if (!tmp)
						break;
					cpu_data = tmp->data;
				} while (cpu != cpu_data->cpu_no);

			}

			cpu_data->cpu_no = cpu;

			it = (idle - cpu_data->idle_time_last);
			kt = (system - cpu_data->system_time_last);
			ut = (user - cpu_data->user_time_last);

			cpu_data->idle_time_percent   = min(max(0.0f, (float)it), 100.0f);
			cpu_data->system_time_percent = min(max(0.0f, (float)kt), 100.0f);
			cpu_data->user_time_percent   = min(max(0.0f, (float)ut), 100.0f);

			cpu_data->idle_time_last   = idle;
			cpu_data->system_time_last = system;
			cpu_data->user_time_last   = user;
		}

		tmp = g_slist_next(tmp);
	}
	free(line);
	fclose(fp);
}


void system_cpu_free(struct system_cpu *system_cpu)
{
	g_slice_free1(sizeof(struct system_cpu), system_cpu);
}
