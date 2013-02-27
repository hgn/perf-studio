#ifndef RANDOM_H
#define RANDOM_H

#include "perf-studio.h"


void rand_init(struct ps *ps);
void rand_free(struct ps *ps);
gchar *rand_hex_string(struct ps *ps, gsize length);

#endif
