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
	struct system_cpu *system_cpu;

	(void) ps;

	system_cpu = g_slice_alloc0(sizeof(*system_cpu));

	return system_cpu;
}


void system_cpu_start(struct ps *ps, struct system_cpu *system_cpu)
{
	int ret;

	ret = clock_gettime(CLOCK_REALTIME, &system_cpu->last_checkpointed);
	if (ret != 0)
		err_msg_die(ps, EXIT_FAILURE, "cannot get system time!");

	system_cpu->clock_tick = sysconf(_SC_CLK_TCK);
	if (system_cpu->clock_tick < 0)
		err_msg_die(ps, EXIT_FAILURE, "Cannot get SC_CLK_TCK - needs fix!");

}


void system_cpu_checkpoint(struct ps *ps, struct system_cpu *system_cpu,
		           struct system_cpu_info *system_cpu_info)
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	int it, kt, ut, cpu;
	long unsigned int user, system, idle;

	assert(system_cpu_info);

	fp = fopen("/proc/stat", "r");
	if (fp == NULL) {
		err_msg_die(ps, EXIT_FAILURE, "Cannot open /proc/stat - urrghl!");
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		line[read - 1] = '\0';

		if (read < 5)
			continue;

		if (strncmp(line, "cpu", 3))
			continue;

		if (!isdigit(line[3]))
			continue;

		if (sscanf (line, "cpu%d %lu %*d %lu %ld", &cpu, &user, &system, &idle) == 4) {

			it = (idle * 1000 / system_cpu->clock_tick * 1000);	/* Idle Time in microseconds */
			kt = (system * 1000 / system_cpu->clock_tick * 1000);	/* Kernel Time in microseconds */
			ut = (user * 1000 / system_cpu->clock_tick * 1000);	/* User Time in microseconds */
			fprintf(stderr, "\"%s\"\n", line);
			fprintf(stderr, "CPU:%d %d %d %d \n", cpu, it, kt, ut);
		}
	}

	free(line);
	fclose(fp);
}


void system_cpu_free(struct system_cpu *system_cpu)
{
	g_slice_free1(sizeof(struct system_cpu), system_cpu);
}
