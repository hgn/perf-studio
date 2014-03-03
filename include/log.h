#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#define LOG_CRITICAL 0
#define LOG_ERROR    1
#define LOG_WARNING  2
#define LOG_INFO     3
#define LOG_DEBUG    4
#define LOG_INVALID  5


#define LOG_VERBOSE 0
#define LOG_CONCISE 1

void log_set_logfile(char *);
int log_get_mode();
void log_set_mode(int);
void logger_set_level(int level);

#define log_print(level, ...) do {				\
	_print_log(level, __FILE__, __func__, __LINE__, __VA_ARGS__); \
} while (0)

extern void _print_log(int, const char *, const char *, int, const char *, ...);

#endif
