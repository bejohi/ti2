#include <errno.h>
#include <stddef.h>
#include "rpl.h"

struct item * rpl_get_operand(struct rpl_context *context)
{
	if (!context) {
		errno = EINVAL;
		return NULL;
	}

	if (!context->stack) {
		errno = ENODATA;
		return NULL;
	}

	while (rpl_needs_eval(context->stack)) {
		if (rpl_eval(context)) {
			return NULL;
		}
	}

	struct item *res = context->stack;
	context->stack = res->next;
	res->next = NULL;
	return res;
}
