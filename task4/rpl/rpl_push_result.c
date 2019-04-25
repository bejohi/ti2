#include <errno.h>
#include <stdlib.h>
#include "rpl.h"

struct item * rpl_push_result(struct rpl_context *context)
{
	if (!context) {
		errno = EINVAL;
		return NULL;
	}

	if ((!context->free) && (rpl_increase_stack(context))) {
		return NULL;
	}

	struct item *top = context->free;
	context->free = top->next;
	top->next = context->stack;
	context->stack = top;
	return top;
}
