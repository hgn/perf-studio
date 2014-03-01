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

#include "shared.h"

#define MAXERRMSG 1024

void pr_warning(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs("\n", stderr);
}

void pr_err(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs("\n", stderr);
}

void pr_debug(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs("\n", stderr);
}

void msg(const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	vfprintf(stderr, format, ap);
	va_end(ap);

	fputs("\n", stderr);
}


static void err_doit(int sys_error, const char *file, const int line_no,
		const char *fmt, va_list ap)
{
	int errno_save;
	char buf[MAXERRMSG];

	errno_save = errno;

	vsnprintf(buf, sizeof buf -1, fmt, ap);
	if (sys_error) {
		size_t len = strlen(buf);
		snprintf(buf + len,  sizeof buf - len, " (%s)", strerror(errno_save));
	}

	fprintf(stderr, "ERROR [%9s:%3d]: %s\n", file, line_no, buf);
	fflush(NULL);
}


void x_err_ret(const char *file, int line_no, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, file, line_no, fmt, ap);
	va_end(ap);
	return;
}


void x_err_sys(const char *file, int line_no, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, file, line_no, fmt, ap);
	va_end(ap);
}


void *xmalloc(size_t size)
{
	void *ptr = malloc(size);
	if (!ptr)
		err_msg_die(EXIT_FAILURE, "Out of mem: %s!\n", strerror(errno));
	return ptr;
}


void *xzalloc(size_t size)
{
	void *ptr = xmalloc(size);
	memset(ptr, 0, size);
	return ptr;
}


char *xstrdup(const char *s)
{
	char *x = strdup(s);
	if (!x)
		err_msg_die(EXIT_FAILURE, "Out of mem: %s!\n", strerror(errno));

	return x;
}


int xsnprintf(char *str, size_t size, const char *format, ...)
{
	va_list ap;
	int len;

	va_start(ap, format);
	len = vsnprintf(str, size, format, ap);
	va_end(ap);

	if (len < 0 || ((size_t)len) >= size)
		err_msg_die(EXIT_FAILURE, "buflen %u not sufficient (ret %d)",
				size, len);
	return len;
}


unsigned long long xstrtoull(const char *str)
{
	char *endptr;
	long long val;

	errno = 0;
	val = strtoll(str, &endptr, 10);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		err_sys_die(EXIT_FAILURE, "strtoll failure");
	}

	if (endptr == str) {
		err_msg_die(EXIT_FAILURE, "No digits found in commandline");
	}

	return val;
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
