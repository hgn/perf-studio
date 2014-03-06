#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>

#include <log.h>


static const char *log_tags[2][6] =
{
	{
		"[CRITICAL] ",
		"[ERROR]    ",
		"[WARNING]  ",
		"[INFO]     ",
		"[DEBUG]    ",
		"[INVALID LOG LEVEL] "
	},
	{
		"[!] ",
		"[*] ",
		"[+] ",
		"[-] ",
		"[.] ",
		"[~] "
	}
};

#define FG_RED		"\033[31m"
#define FG_YELLOW	"\033[33m"
#define FG_BLUE		"\033[34m"
#define FG_PURPLE	"\033[35m"
#define FG_GREEN	"\033[32m"
#define TEXT_BOLD	"\033[1m"
#define COLOR_END	"\033[0m"

static FILE *log_file;
static int current_log_mode = 1;
static int  _log_current_mode = 1;

void log_set_logfile(char *filename)
{
	if (log_file) {
		fclose(log_file);
	}

	log_file = fopen(filename, "a+");
}

static int verbose = 0;

void log_set_verbose(void)
{
	verbose = 1;
}


void log_set_mode(int mode)
{
	current_log_mode = mode;
}

int log_get_mode(void)
{
	return current_log_mode;
}

static int log_level = LOG_INFO;

void log_set_level(int level)
{
	assert(level >= 0 && level <=  LOG_INVALID);
	log_level = level;
}


__attribute__((__format__ (__printf__, 5, 0)))
void _print_log(int loglevel, const char *file, const char *func,
		int line, const char *msg, ...)
{
	va_list args;
	char *buffer;

	if (loglevel > log_level + 2)
		return;

	log_file = stdout;

	va_start(args, msg);

	// append the varargs to the buffer we want to print.
	// this is so that our pipe chars don't get fucked later.
	// also, make sure we don't get an invalid loglevel.
	buffer = alloca(strlen(msg) + 1024);
	vsnprintf(buffer, strlen(msg) + 1024, msg, args);
	if (loglevel < 0 || loglevel > 4)
		loglevel = LOG_INVALID;

	// set console color for printing the tag
	switch (loglevel) {
		case LOG_CRITICAL:
			printf(TEXT_BOLD FG_RED);
			break;
		case LOG_ERROR:
			printf(FG_RED);
			break;
		case LOG_WARNING:
			printf(FG_YELLOW);
			break;
		case LOG_INFO:
			printf(FG_BLUE);
			break;
		case LOG_DEBUG:
			printf(FG_GREEN);
			break;
		case LOG_INVALID:
			printf(FG_PURPLE);
			break;
	}
	if (log_file)
		fprintf(log_file, "%s", log_tags[_log_current_mode][loglevel]);
	printf(COLOR_END);

	if (verbose)
		fprintf(log_file, "%s:%04d:%s():", file, line, func);

	fprintf(log_file, " %s\n", buffer);

	va_end(args);
}
