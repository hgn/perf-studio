#ifndef PERF_STUDIO_H
#define	PERF_STUDIO_H

#include <inttypes.h>

#define min(x,y) ({                     \
        typeof(x) _x = (x);             \
        typeof(y) _y = (y);             \
        (void) (&_x == &_y);    \
        _x < _y ? _x : _y; })

#define max(x,y) ({                     \
        typeof(x) _x = (x);             \
        typeof(y) _y = (y);             \
        (void) (&_x == &_y);    \
        _x > _y ? _x : _y; })

#if !defined likely && !defined unlikely
# define likely(x)   __builtin_expect(!!(x), 1)
# define unlikely(x) __builtin_expect(!!(x), 0)
#endif

#define	SEED_DEVICE "/dev/urandom"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

struct args {
	char *me;
	int verbose;
};

struct ps_ctx {
	struct args args;
};

void pr_warning(const char *format, ...);
void pr_err(const char *format, ...);
void pr_debug(const char *format, ...);


#endif

