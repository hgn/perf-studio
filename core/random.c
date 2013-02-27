#include "random.h"
#include "shared.h"

void rand_init(struct ps *ps)
{
	pr_info(ps, "initialize PRNG");
	ps->rand = g_rand_new();
}

void rand_free(struct ps *ps)
{
	g_rand_free(ps->rand);
}


