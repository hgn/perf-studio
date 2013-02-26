#ifndef MODULES_H
#define MODULES_H

#include "perf-studio.h"

int register_available_modules(struct ps *ps);
void unregister_all_modules(struct ps *ps);

#endif
