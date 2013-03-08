#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <assert.h>

#include "perf-studio.h"
#include "str-parser.h"


void str_parser_init(struct str_parser *str_parser, const char *new_str)
{
	assert(str_parser);
	assert(new_str);
	assert(*new_str);

	memset(str_parser, 0, sizeof(*str_parser));
	str_parser->start_ptr = new_str;
	str_parser->curr_ptr  = new_str;
}


void str_parser_reset(struct str_parser *str_parser)
{
	assert(str_parser);
	str_parser->curr_ptr = str_parser->start_ptr;
}


int str_parser_skip_spaces(struct str_parser *str_parser)
{
	assert(str_parser);

	if (!*str_parser->curr_ptr)
		return STR_PARSER_RET_EOL;

	while (isspace(*str_parser->curr_ptr))
		str_parser->curr_ptr++;

	if (!*str_parser->curr_ptr)
		return STR_PARSER_RET_EOL;

	return STR_PARSER_RET_SUCCESS;
}


int str_parser_long(struct str_parser *str_parser, long *retval)
{
	long val;
	char *endptr;

	assert(str_parser);

	if (!*str_parser->curr_ptr)
		return STR_PARSER_RET_EOL;

	errno = 0;
	val = strtol(str_parser->curr_ptr, &endptr, 10);
	if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN))
			|| (errno != 0 && val == 0)) {
		return STR_PARSER_RET_EXCEPTION;
	}

	if (endptr == str_parser->curr_ptr)
		return STR_PARSER_RET_INVALID;

	*retval = val;
	str_parser->curr_ptr = endptr;

	return STR_PARSER_RET_SUCCESS;
}


int str_parser_next_long(struct str_parser *str_parser, long *retval)
{
	int ret;

	assert(str_parser);

	/* skip potential leading whitespaced */
	ret = str_parser_skip_spaces(str_parser);
	if (ret != STR_PARSER_RET_SUCCESS)
		return ret;

	ret = str_parser_long(str_parser, retval);
	if (ret != STR_PARSER_RET_SUCCESS)
		return ret;

	return STR_PARSER_RET_SUCCESS;
}


int str_parser_next_alphanum(struct str_parser *str_parser, char *dest, size_t max_len)
{
	int ret;
	ptrdiff_t delta;
	const char *newptr;

	/* skip potential leading whitespaced */
	ret = str_parser_skip_spaces(str_parser);
	if (ret != STR_PARSER_RET_SUCCESS)
		return ret;

	newptr = str_parser->curr_ptr;

	while (isalnum(*newptr)) {
		newptr++;
	}

	if (newptr == str_parser->curr_ptr)
		return STR_PARSER_RET_INVALID;

	delta = newptr - str_parser->curr_ptr;
	delta = min(delta, (ptrdiff_t)max_len - 1);
	memcpy(dest, str_parser->curr_ptr, delta);
	dest[delta] = '\0';

	str_parser->curr_ptr = newptr;

	return STR_PARSER_RET_SUCCESS;
}


int str_parser_remain(struct str_parser *str_parser, char *dest, size_t max_len)
{
	size_t delta;

	delta = min(strlen(str_parser->curr_ptr), max_len - 1);

	memcpy(dest, str_parser->curr_ptr, delta);
	dest[delta] = '\0';

	str_parser->curr_ptr += delta;

	return STR_PARSER_RET_SUCCESS;
}
