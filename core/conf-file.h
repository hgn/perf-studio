#ifndef CONF_FILE_H
#define CONF_FILE_H

#include "perf-studio.h"

int load_user_conf_file(struct ps *);
int load_projects_from_cache(struct ps *);
void conf_file_update_project_last_used(struct ps *ps, struct project *project);

#endif
