#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "rpl.h"

static const size_t MIN_SIZE = 64;

int rpl_increase_stack(struct rpl_context *context)
{
	size_t n_items;
	struct item *items;

	if (!context) {
		errno = EINVAL;
		return -1;
	}

	n_items = context->stack_size << 1;
	if (n_items < MIN_SIZE) {
		n_items = MIN_SIZE;
	}

	items = malloc(sizeof(struct item) * n_items);
	if (!items) {
		errno = ENOMEM;
		return -1;
	}

	// Initialize all new items with zero
	memset(items, 0x00, sizeof(struct item) * n_items);
	items[0].next = context->free;

	for (size_t i = 1; i < n_items; i++) {
		items[i].next = &items[i - 1];
	}

	context->free = &items[n_items - 1];
	context->stack_size += n_items;
	return 0;
}
