#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "rpl.h"

static char * tokenize_regular(char **str)
{
	char *pos, *res;
	pos = res = *str;

	while (1) {
		switch (*pos) {
			case ' ':
				*pos = '\0';
				*str = pos + 1;;
				return res;
			case '\0':
				*str = pos;
				return res;
			case '\n':
				*pos = '\0';
				*str = pos;
				return res;
			default:
				break;
		}
		pos++;
	}
}

static char * tokenize_quote(char **str)
{
	char *pos, *res, *dest;
	dest = pos = res = *str + 1;

	while (1) {
		switch (*pos) {
			case '\\':
				switch (*(++pos)) {
					case '\"':
						*dest = '\"';
						break;
					case 'n':
						*dest = '\n';
						break;
					case '\\':
						*dest = '\\';
						break;
					default:
						fputs("Invalid input: Unsupported escape sequence\n",
						      stderr);
						return NULL;
				}
				break;
			case '\"':
				*dest = '\0';
				switch (*(++pos)) {
					case ' ':
						*str = ++pos;
						return res;
					case '\0':
						*str = pos;
						return res;
					case '\n':
						*str = ++pos;
						return res;
				}
				fputs("Invalid input: Quote (\"...\") not properly "
				      "terminated\n", stderr);
				return NULL;
			case '\0':
				fputs("Invalid input: Quote (\"...\") not terminated. "
				      "(Missing \")\n", stderr);
				return NULL;
			default:
				*dest = *pos;
				break;
		}
		dest++;
		pos++;
	}
}

static char * tokenize(char **str)
{
	switch (**str) {
		case '\0':
			return NULL;
		case '\"':
			return tokenize_quote(str);
	}

	return tokenize_regular(str);
}

int main(void)
{
	struct rpl_context context = {
		.c_funcs = c_funcs_default
	};

	while (1) {
		char buf[128];
		char *line = buf;
		size_t len = sizeof(buf);
		rpl_clear_stack(&context);
		fputs("> ", stdout);
		fflush(stdout);
		if (getline(&line, &len, stdin) == -1) {
			perror("Failed to read input");
			return EXIT_FAILURE;
		}
		char *token;
		while ((token = tokenize(&line))) {
			rpl_parse(&context, token);
		}

		while (context.stack && rpl_needs_eval(context.stack)) {
			if (rpl_eval(&context)) {
				perror("rpl_eval() failed");
				continue;
			}
		}

		puts("Result:");
		while (context.stack) {
			rpl_print(context.stack);
			rpl_free_top(&context);
		}
	}

	return EXIT_SUCCESS;
}
