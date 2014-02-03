#ifndef CONF_FILE_H
#define CONF_FILE_H

#include "perf-studio.h"

int load_user_conf_file(struct ps *);
int load_projects_from_cache(struct ps *);
void project_conf_file_update_last_used(struct ps *ps, gchar *project_path);

#endif
