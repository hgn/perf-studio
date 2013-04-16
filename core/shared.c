#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "perf-studio.h"

#include "shared.h"

#define MAXERRMSG 1024


void msg(struct ps *ps, int level, const char *format, ...)
{
	va_list ap;

	(void) ps;
	(void) level;

	fputs("# ", stderr);

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs("\n", stderr);
}


static void err_doit(struct ps *ps, int sys_error, const char *file, const int line_no,
		const char *fmt, va_list ap)
{
	int errno_save;
	char buf[MAXERRMSG];

	(void) ps;
	errno_save = errno;

	vsnprintf(buf, sizeof buf -1, fmt, ap);
	if (sys_error) {
		size_t len = strlen(buf);
		snprintf(buf + len,  sizeof buf - len, " (%s)", strerror(errno_save));
	}

	fprintf(stderr, "! ERROR [%9s:%3d]: %s\n", file, line_no, buf);
	fflush(NULL);
}


void x_err_ret(struct ps *ps, const char *file, int line_no, const char *fmt, ...)
{
	va_list ap;
	(void) ps;

	va_start(ap, fmt);
	err_doit(ps, 0, file, line_no, fmt, ap);
	va_end(ap);
	return;
}


void x_err_sys(struct ps *ps, const char *file, int line_no, const char *fmt, ...)
{
	va_list ap;
	(void) ps;

	va_start(ap, fmt);
	err_doit(ps, 1, file, line_no, fmt, ap);
	va_end(ap);
}


int xatoi(const char *str, int *retval)
{
	long val;
	char *endptr;

	val = strtol(str, &endptr, 10);
	if ((val == LONG_MIN || val == LONG_MAX) && errno != 0)
		return -EINVAL;

	if (endptr == str)
		return -EINVAL;

	if (val > INT_MAX)
		return -EINVAL;
	else if (val < INT_MIN)
		return -EINVAL;

	if ('\0' != *endptr)
		return -EINVAL;

	*retval = val;

	return 0;
}


void get_random_bytes(char *data, size_t len)
{
	size_t i;

	if (len == 0 || data == NULL)
		return;

	for (i = 0; i < len; i++)
		data[i] = (char) random();
}


size_t strlcpy(char *dest, const char *src, size_t size)
{
	size_t ret = strlen(src);

	if (size) {
		size_t len = (ret >= size) ? size - 1 : ret;
		memcpy(dest, src, len);
		dest[len] = '\0';
	}

	return ret;
}

