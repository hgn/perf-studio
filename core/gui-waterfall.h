#ifndef GUI_WATERFALL_H
#define GUI_WATERFALL_H

#include "perf-studio.h"
#include "ringbuffer.h"

struct cpu_waterfall {
	long no_cpu;
	size_t ring_buffer_elements;
	struct ring_buffer *ring_buffer;
	size_t max_elements;
};

/* forward declaration */
struct system_cpu;

struct cpu_waterfall *cpu_waterfall_new(struct ps *);
void cpu_waterfall_add_data(struct ps *, struct cpu_waterfall *, struct system_cpu *);
void cpu_waterfall_free(struct cpu_waterfall *);
void cpu_waterfall_zero_color(struct ps_color *color);

/*
 * To keep the API similar we name the function similar!
 * Not sure if this right, because the semantic is different
 * here: the checkpointing is already done. CPU waterfall
 * just update the data, thats all. So I am not sure ...
 */
static inline void cpu_waterfall_update(struct ps *ps, struct cpu_waterfall *cw,
				       struct system_cpu *sc)
{
	cpu_waterfall_add_data(ps, cw, sc);
}

#endif
