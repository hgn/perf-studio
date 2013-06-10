#ifndef SHARED_H
#define	SHARED_H

#if 0

pr_debug();

- always console

pr_info()

- always console

pr_warning()
pr_warning_sys()

- default
- always console

pr_error()

- console

pr_error_die()
pr_error_sys_die()

- console or X
- file/line information is displayed


#endif

#include <inttypes.h>

#include "perf-studio.h"

#define EXIT_INTERNAL 2
#define EXIT_SYS_FAIL 3
#define EXIT_COMMANDLINE 4

#define alloc_nr(x) (((x) + 16) * 3 / 2)
/*
 * Realloc the buffer pointed at by variable 'x' so that it can hold
 * at least 'nr' entries; the number of entries currently allocated
 * is 'alloc', using the standard growing factor alloc_nr() macro.
 *
 * DO NOT USE any expression with side-effect for 'x' or 'alloc'.
 */
#define ALLOC_GROW(x, nr, alloc) \
	do { \
		if ((nr) > alloc) { \
			if (alloc_nr(alloc) < (nr)) \
			alloc = (nr); \
			else \
			alloc = alloc_nr(alloc); \
			x = realloc((x), alloc * sizeof(*(x))); \
		} \
	} while(0)

/* error handling */
#define err_msg(ps, format, args...) \
        do { \
                x_err_ret(ps, __FILE__, __LINE__,  format , ## args); \
        } while (0)

#define err_sys(ps, format, args...) \
        do { \
                x_err_sys(ps, __FILE__, __LINE__,  format , ## args); \
        } while (0)

#define err_sys_die(ps, exitcode, format, args...) \
        do { \
                x_err_sys(ps, __FILE__, __LINE__, format , ## args); \
                exit(exitcode); \
        } while (0)

#define err_msg_die(ps, exitcode, format, args...) \
        do { \
                x_err_ret(ps, __FILE__, __LINE__,  format , ## args); \
                exit(exitcode); \
        } while (0)


#define pr_debug(ps, format, args...) \
        do { \
                msg(ps, MSG_LEVEL_DEBUG, format , ## args); \
        } while (0)

#define pr_info(ps, format, args...) \
        do { \
                msg(ps, MSG_LEVEL_INFO, format , ## args); \
        } while (0)

#define pr_warn(ps, format, args...) \
        do { \
                msg(ps, MSG_LEVEL_WARNING, format , ## args); \
        } while (0)

#define pr_error(ps, format, args...) \
        do { \
                msg(ps, MSG_LEVEL_ERROR, format , ## args); \
        } while (0)

void x_err_ret(struct ps *ps, const char *file, int line_no, const char *, ...);
void x_err_sys(struct ps *ps, const char *file, int line_no, const char *, ...);
int xatoi(const char *, int *);
void msg(struct ps *ps, int level, const char *, ...);
void get_random_bytes(char *, size_t);
size_t strlcpy(char *dest, const char *src, size_t size);

#endif /* SHARED_H */
