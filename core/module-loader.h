#ifndef MODULE_LOADER_H
#define MODULE_LOADER_H

#include "perf-studio.h"

int register_available_modules(struct ps *ps);
void unregister_all_modules(struct ps *ps);
void module_activated_by_name(struct ps *ps, const char *module_name);

#endif
