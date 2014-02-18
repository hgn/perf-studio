#ifndef EXECUTER_H
#define EXECUTER_H


#include "perf-studio.h"
#include "event.h"

void executer_register_module_events(struct ps *ps, struct module *module);
void executer_unregister_module_events(struct ps *ps, struct module *module);
void execute_module_triggered_analyze(struct module *module);


#endif /* EXECUTER_H */
