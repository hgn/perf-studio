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
#include "str-parser.h"
#include "shared.h"

struct system_cpu *system_cpu_new(struct ps *ps)
{
	int ret;
	long no_cpus, i;
	struct system_cpu *system_cpu;

	system_cpu = g_slice_alloc0(sizeof(*system_cpu));

	ret = clock_gettime(CLOCK_REALTIME, &system_cpu->last_updated);
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


void system_cpu_update(struct ps *ps, struct system_cpu *system_cpu)
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

		if (sscanf(line, "cpu%lu %lu %*d %lu %lu", &cpu, &user, &system, &idle) == 4) {
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


struct interrupt_monitor_data *interrupt_monitor_data_new(struct ps *ps)
{
	struct interrupt_monitor_data *imd;

	imd = g_slice_alloc0(sizeof(*imd));
	imd->proc_interrupts_fh = fopen("/proc/interrupts", "r");
	if (!imd->proc_interrupts_fh) {
		pr_warn(ps, "Cannot open /proc/interrupts");
		goto err;
	}
	imd->interrupt_data_array = g_array_new(FALSE, FALSE, sizeof(struct interrupt_data));
	/* al cheapo flog to signal un/initialized state[TM] */
	imd->start_time.tv_sec = 0;

	return imd;
err:
	g_slice_free1(sizeof(struct interrupt_monitor_data), imd);
	return NULL;
}


void interrupt_monitor_ctrl_free(struct interrupt_monitor_data *imd)
{
	g_array_free(imd->interrupt_data_array, TRUE);
	if (imd->proc_interrupts_fh)
		fclose(imd->proc_interrupts_fh);
	g_slice_free1(sizeof(struct interrupt_monitor_data), imd);
}

void interrupt_monitor_ctrl_update(struct ps *ps, struct interrupt_monitor_data *imd)
{
	int ret;
	unsigned i, j;
	char *line = NULL;
	size_t size = 0;
	ssize_t read;
	gboolean initial_phase = FALSE;

	assert(imd);
	assert(imd->proc_interrupts_fh);

	ret = fseek(imd->proc_interrupts_fh, SEEK_SET, 0);
	if (ret != 0) {
		pr_warn(ps, "Could net fseek to 0 for /proc/interrupts");
		return;
	}

	/* skip cpu line */
	read = getline(&line, &size, imd->proc_interrupts_fh);
	if (read == -1) {
		free(line); line = NULL;
		return;
	}

	i = 0;
	while (!feof(imd->proc_interrupts_fh)) {
		struct str_parser str_parser;
		struct interrupt_data *interrupt_data;
		char buffer[8];
		long retval;

		read = getline(&line, &size, imd->proc_interrupts_fh);
		if (read == -1) {
			free(line); line = NULL;
			return;
		}

		line[read - 1] = '\0';
		str_parser_init(&str_parser, line);

		str_parser_next_alphanum(&str_parser, buffer, 8);
		if (streq(buffer, "ERR") || streq(buffer, "MIS")) {
			free(line); line = NULL;
			continue;
		}

		if (unlikely(imd->interrupt_data_array->len <= i)) {
			/* array to small, just increase array by one element */
			imd->interrupt_data_array = g_array_set_size(imd->interrupt_data_array, i + 1);
			interrupt_data = &g_array_index(imd->interrupt_data_array, struct interrupt_data, i);
			interrupt_data->irq_array = g_array_new(FALSE, FALSE, sizeof(struct irq_start_current));
			memcpy(interrupt_data->name, buffer, 8);
			initial_phase = TRUE;
		}  else {
			/* get the existing struct */
			interrupt_data = &g_array_index(imd->interrupt_data_array, struct interrupt_data, i);
		}

		str_parser_skip_char(&str_parser, ':');
		j = 0;
		do {
			struct irq_start_current *irq_start_current;

			ret = str_parser_next_long(&str_parser, &retval);
			if (ret == STR_PARSER_RET_INVALID)
				break;

			if (unlikely(interrupt_data->irq_array->len <= j)) {
				interrupt_data->irq_array = g_array_set_size(interrupt_data->irq_array, j + 1);
				irq_start_current = &g_array_index(interrupt_data->irq_array,  struct irq_start_current, j);
				irq_start_current->start = irq_start_current->current = retval;
			} else {
				irq_start_current = &g_array_index(interrupt_data->irq_array,  struct irq_start_current, j);
				irq_start_current->current = retval;
			}

			j++;
		} while (1);

		/*
		 * now we store the remaining interrupt description
		 * if we parse the line the first time
		 */
		if (initial_phase)
			str_parser_remain(&str_parser, interrupt_data->description, sizeof(interrupt_data->description));

		free(line); line = NULL;
		i++;
	}

	if (!imd->start_time.tv_sec)
		clock_gettime(CLOCK_REALTIME, &imd->start_time);

	free(line);
}
