#include "random.h"
#include "shared.h"

gchar *rand_hex_string(struct ps *ps, gsize length)
{
	gsize i;
	guint32 ret;
	gchar *ret_str;

	ret_str = g_malloc(length + 1);

	for (i = 0; i < length; i++) {
		ret = g_rand_int_range(ps->rand, 0, 17);
		sprintf(&ret_str[i], "%x", ret);
	}

	ret_str[i] = '\0';
	return ret_str;
}


void rand_init(struct ps *ps)
{
	pr_info(ps, "initialize PRNG");
	ps->rand = g_rand_new();
}

void rand_free(struct ps *ps)
{
	g_rand_free(ps->rand);
}


