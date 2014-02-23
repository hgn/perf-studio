#ifndef EXECUTER_H
#define EXECUTER_H


#include "perf-studio.h"

void execute_module_triggered_analyze(struct module *module);
int executer_init(struct ps *ps);
void executer_fini(struct ps *ps);


#endif /* EXECUTER_H */
