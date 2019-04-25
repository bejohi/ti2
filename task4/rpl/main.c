#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "rpl.h"

static char * tokenize(char **str)
{
	char *pos, *res;
	pos = res = *str;
	if (*pos == '\0') {
		return NULL;
	}

	while (1) {
		switch(*pos) {
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

		fputs("Result: ", stdout);
		rpl_print(context.stack);
	}

	return EXIT_SUCCESS;
}
