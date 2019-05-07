#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "rpl.h"

static int impl(struct rpl_context *context) {
	struct item *lhs, *rhs, *res;

	if (!context) {
		errno = EINVAL;
		return -1;
	}

	rhs = rpl_get_operand(context);
	if (!rhs) {
		return -1;
	}

	lhs = rpl_get_operand(context);
	if (!lhs) {
		return -1;
	}

	res = rpl_push_result(context);
	if (!res) {
		return -1;
	}

	if ((lhs->type == TYPE_STRING) || (rhs->type == TYPE_STRING)) {
		fputs("Multiplication with strings is undefined\n", stderr);
		errno = EINVAL;
		return -1;
	}

	res->type = TYPE_NUMBER;
	res->value.number = lhs->value.number / rhs->value.number;

	return 0;
}

const struct c_func rpl_div = {
	.name = "/",
	.impl = impl,
};
