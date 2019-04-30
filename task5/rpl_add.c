#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rpl.h"

static const size_t NUMBER_STRLEN = 20;

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
		size_t len = 1;
		switch (lhs->type) {
			case TYPE_STRING:
				len += strlen(lhs->value.str);
				break;
			case TYPE_NUMBER:
				len += NUMBER_STRLEN;
				break;
			default:
				errno = EINVAL;
				return -1;
		}

		switch (rhs->type) {
			case TYPE_STRING:
				len += strlen(rhs->value.str);
				break;
			case TYPE_NUMBER:
				len += NUMBER_STRLEN;
				break;
			default:
				errno = EINVAL;
				return -1;
		}

		res->type = TYPE_STRING;
		char *str;
		res->value.str = str = malloc(len);
		if (!res->value.str) {
			errno = ENOMEM;
			return -1;
		}

		if (lhs->type == TYPE_STRING) {
			if (rhs->type == TYPE_STRING) {
				// string - string
				snprintf(str, len, "%s%s",
				         lhs->value.str, rhs->value.str);
			}
			else {
				// string - number
				snprintf(str, len, "%s%" PRId64,
				         lhs->value.str, rhs->value.number);
			}
		}
		else {
			// number - string
			snprintf(str, len, "%" PRId64 "%s",
			         lhs->value.number, rhs->value.str);
		}
	}
	else {
		if ((lhs->type != TYPE_NUMBER) || (rhs->type != TYPE_NUMBER)) {
			errno = EINVAL;
			return -1;
		}
		// number - number
		res->type = TYPE_NUMBER;
		res->value.number = lhs->value.number + rhs->value.number;
	}

	return 0;
}

const struct c_func rpl_add = {
	.name = "+",
	.impl = impl,
};
