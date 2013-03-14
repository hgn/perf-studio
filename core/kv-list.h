#ifndef KV_LIST_H
#define KV_LIST_H

#include "perf-studio.h"


enum {
        KV_LIST_TYPE_INT_STRING,
        KV_LIST_TYPE_MAX
};

struct kv_list_entry {
        void *key;
        void *value;
};

struct kv_list {
        int type;
        GSList *kv_list_entry_list;
};


#endif /* KV_LIST_H */

