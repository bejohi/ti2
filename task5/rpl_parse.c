#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "rpl.h"

static struct item * push_item(struct rpl_context *context)
{
	if ((!context->free) && (rpl_increase_stack(context))) {
		return NULL;
	}
	struct item *tmp = context->free;
	context->free = tmp->next;
	tmp->next = context->stack;
	context->stack = tmp;
	return tmp;
}

static int parse_number(struct rpl_context *context, const char *str)
{
	char *end;
	int64_t num = strtoll(str, &end, 0);
	if ((end == str) || (*end != '\0')) {
		return 1;
	}

	struct item *top = push_item(context);
	if (!top) {
		return -1;
	}
	top->type = TYPE_NUMBER;
	top->value.number = num;
	return 0;
}

int rpl_parse(struct rpl_context *context, const char *str)
{
	if (!context || !str) {
		errno = EINVAL;
		return -1;
	}

	size_t pos;
	const struct c_func *i;
	for (pos = 0, i = context->c_funcs[0]; (i); i = context->c_funcs[++pos]) {
		if (strcmp(i->name, str) == 0) {
			// Given string matches a c function
			struct item *top = push_item(context);
			if (!top) {
				return -1;
			}
			top->type = TYPE_C_FUNC;
			top->value.c_func = i;
			return 0;
		}
	}

	switch (parse_number(context, str)) {
		case -1:
			return -1;
		case 0:
			// Was a number, done
			break;
		case 1:
			{
				// str is also not a number, must be a string
				struct item *top = push_item(context);
				if (!top) {
					return -1;
				}
				size_t len = strlen(str);
				char *s;
				if (!(top->value.str = s = malloc(len + 1))) {
					errno = ENOMEM;
					return -1;
				}
				memcpy(s, str, len + 1);
				top->type = TYPE_STRING;
			}
	}

	return 0;
}
