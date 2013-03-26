#ifndef MODULE_UTILS_H
#define MODULE_UTILS_H


#include "perf-studio.h"

struct events;

/* module basic operations */
struct module *module_new(void);
void module_free(struct module *m);
void module_set_name(struct module *m, const char *name);
char *module_get_name(struct module *m);
void module_set_description(struct module *m, const char *desc);
char *module_get_description(struct module *m);

/* event handling */
int module_add_events(struct module *m, struct events *e);


#endif
