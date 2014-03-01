#ifndef SHARED_H
#define	SHARED_H

#include <inttypes.h>

/* error handling */
#define err_msg(format, args...) \
        do { \
                x_err_ret(__FILE__, __LINE__,  format , ## args); \
        } while (0)

#define err_sys(format, args...) \
        do { \
                x_err_sys(__FILE__, __LINE__,  format , ## args); \
        } while (0)

#define err_sys_die(exitcode, format, args...) \
        do { \
                x_err_sys(__FILE__, __LINE__, format , ## args); \
                exit(exitcode); \
        } while (0)

#define err_msg_die(exitcode, format, args...) \
        do { \
                x_err_ret(__FILE__, __LINE__,  format , ## args); \
                exit(exitcode); \
        } while (0)

void x_err_ret(const char *file, int line_no, const char *, ...);
void x_err_sys(const char *file, int line_no, const char *, ...);
void *xmalloc(size_t len);
void *xzalloc(size_t len);
char * xstrdup(const char *);
int xsnprintf(char *, size_t , const char *, ...);
unsigned long long xstrtoull(const char *);
int xatoi(const char *, int *);
void msg(const char *, ...);
void get_random_bytes(char *, size_t);

#endif /* SHARED_H */
