#ifndef STR_PARSER_H
#define STR_PARSER_H

#define STR_PARSER_RET_SUCCESS    0
#define STR_PARSER_RET_EOL       -1
#define STR_PARSER_RET_INVALID   -2
#define STR_PARSER_RET_EXCEPTION -3
#define STR_PARSER_RET_NOBUFS    -4


struct str_parser {
	const char const *start_ptr;
	const char *curr_ptr;
};

// FIXME rename to _create
void str_parser_init(struct str_parser *str_parser, const char *new_str);
void str_parser_reset(struct str_parser *str_parser);
int str_parser_skip_spaces(struct str_parser *str_parser);
int str_parser_long(struct str_parser *str_parser, long *retval);
int str_parser_next_long(struct str_parser *str_parser, long *retval);
int str_parser_next_alphanum(struct str_parser *str_parser, char *dest, size_t max_len);
int str_parser_remain(struct str_parser *str_parser, char *dest, size_t max_len);

static inline void str_parser_skip_char(struct str_parser *str_parser, char x)
{
	if (*str_parser->curr_ptr == x)
		str_parser->curr_ptr++;
}


#endif /* STR_PARSER_H */
