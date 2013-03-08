#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

#include <stdio.h>
#include <time.h>

#include "perf-studio.h"

struct cpu_data {
	/* user data */
	float idle_time_percent;
	float system_time_percent;
	float user_time_percent;

	/* internal data */
	guint cpu_no;
	unsigned long idle_time_last;
	unsigned long system_time_last;
	unsigned long user_time_last;
};

struct system_cpu {
	struct timespec last_checkpointed;
	/* index in list is number of cpu */
	GSList *cpu_data_list;
	long clock_tick;
};

struct system_cpu *system_cpu_new(struct ps *);
void system_cpu_free(struct system_cpu *system_cpu);
void system_cpu_checkpoint(struct ps *ps, struct system_cpu *system_cpu);
#define SYSTEM_CPU_NO_CPUS(system_cpu) (g_slist_length(system_cpu->cpu_data_list))


struct interrupt_monitor_data {
	struct timespec start_time;
	FILE *proc_interrupts_fh;
};

struct interrupt_monitor_data *interrupt_monitor_data_new(struct ps *ps);
void interrupt_monitor_ctrl_checkpoint(struct ps *, struct interrupt_monitor_data *);
void interrupt_monitor_ctrl_free(struct interrupt_monitor_data *);

#endif
