#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <time.h>

#include "perf-studio.h"

struct cpu_data {
	guint cpu_no;
	unsigned long idle_time_last;
	unsigned long kernel_time_last;
	unsigned long user_time_last;
};

struct system_cpu {
	struct timespec last_checkpointed;
	/* index in list is number of cpu */
	GSList *cpu_data_list;
	long clock_tick;
};

struct system_cpu_info {
	unsigned long idle_time;
	unsigned long kernel_time;
	unsigned long user_time;
};

struct system_cpu *system_cpu_new(struct ps *);
void system_cpu_free(struct system_cpu *system_cpu);
void system_cpu_start(struct ps *ps, struct system_cpu *system_cpu);
void system_cpu_checkpoint(struct ps *ps, struct system_cpu *system_cpu,
		           struct system_cpu_info *system_cpu_info);

#endif
