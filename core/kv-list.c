#include <assert.h>

#include "kv-list.h"

struct kv_list *kv_list_new(unsigned int type)
{
        struct kv_list *kv_list;

        assert(type < KV_LIST_TYPE_MAX);

        kv_list = g_malloc0(sizeof(*kv_list));
        kv_list->type = type;

        return kv_list;
}

void kv_list_add_int_string(struct kv_list *kv_list, int key, char *value)
{
        struct kv_list_entry *e;

        e = g_malloc(sizeof(*e));
        e->key   = GINT_TO_POINTER(key);
        e->value = value;

        kv_list = g_slist_append(kv_list, e);
}

static void kv_list_free_int_string(struct kv_list *kv_list)
{
        int *type;
        char *value;
        GSList *tmp;

        tmp = kv_list->kv_list_entry_list;
        while (tmp) {
                struct kv_list_entry *e;

                e = tmp->data;
                assert(e);
                g_free(e);
                tmp = g_slist_next(tmp);
        }

        g_slist_free(kv_list->kv_list_entry_list);
}


void kv_list_free(struct kv_list *kv_list)
{
        assert(kv_list);

        switch (kv_list->type) {
        case KV_LIST_TYPE_INT_STRING:
                break;
        default:
                assert(0);
                break;
        }

        g_free(kv_list);
}
