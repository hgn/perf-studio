#ifndef MODULE_UTILS_H
#define MODULE_UTILS_H

#include <assert.h>


#include "perf-studio.h"

struct events;

/* module basic operations */
struct module *module_new(void);
void module_free(struct module *m);
void module_set_name(struct module *m, const char *name);
char *module_get_name(struct module *m);
void module_set_description(struct module *m, const char *desc);
char *module_get_description(struct module *m);
void module_set_group(struct module *m, int group);
int module_get_group(struct module *m);
const char *module_group_str(int id);

/**
 * module_set_maturity - set maturity level for module
 * @module: the affected module
 * @level: the level
 *
 * During development a module is often not in a bug free condition
 * and things may change rapidly or the code is not stable enough.
 * To provide a way to distribute bleeding age module too a module
 * can specify itself as experimental (MODULE_MATURITY_EXPERIMENTAL).
 * Users will only see experimental marked modules if the configuration
 * is enabled.
 */
void module_set_maturity(struct module *module, int level);


/**
 * module_maturity_str - return a user readable string of the maturity level
 * @module: the referenced module
 *
 * Return a string representative of the maturity level form module.
 */
const char *module_maturity_str(struct module *module);

#endif
