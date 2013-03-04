#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <time.h>

#include "perf-studio.h"

struct system_cpu {
	struct timespec last_checkpointed;
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
