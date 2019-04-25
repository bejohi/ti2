#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "rpl.h"

int rpl_eval(struct rpl_context *context)
{
	if (!context) {
		errno = EINVAL;
		return -1;
	}

	struct item *top = context->stack;

	if (!top) {
		fputs("ERR: No data on stack, cannot evaluate\n", stderr);
		errno = EINVAL;
		return -1;
	}

	switch (top->type) {
		case TYPE_C_FUNC:
			{
				int retval;
				context->stack = top->next;
				retval = top->value.c_func->impl(context);
				top->next = context->free;
				context->free = top;
				if (retval) {
					fprintf(stderr, "Execution of \"%s\" failed: %s\n",
					        top->value.c_func->name, strerror(errno));
					return -1;
				}
			}
			break;
		default:
			// No further evaluation needed
			break;
	}

	return 0;
}
