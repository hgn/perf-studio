#ifndef KV_LIST_H
#define KV_LIST_H

#include "perf-studio.h"

#define KV_LIST_HEAD(x) (x->kv_list_entry_list)


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


struct kv_list *kv_list_new(unsigned int type);
void kv_list_add_int_string(struct kv_list *kv_list, int key, char *value);
void kv_list_free(struct kv_list *kv_list);

#endif /* KV_LIST_H */

