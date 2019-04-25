#include <assert.h>
#include <stdlib.h>
#include "rpl.h"

void rpl_free_top(struct rpl_context *context)
{
	struct item *item;
	assert(context && "rpl_free_top() must not be called with NULL");
	if (!context->stack) {
		return;
	}
	item = context->stack;
	context->stack = item->next;

	switch (item->type) {
		case TYPE_STRING:
			free((char *)item->value.str);
			break;
		default:
			// No additional memory was allocated, nothing to free
			break;
	}

	item->next = context->free;
	context->free = item;
}
