#include <inttypes.h>
#include <stdio.h>
#include "rpl.h"

void rpl_print(struct item *i)
{
	if (i) {
		switch(i->type) {
			case TYPE_NUMBER:
				printf("Number: %" PRId64 "\n", i->value.number);
				break;
			case TYPE_STRING:
				printf("String: \"%s\"\n", i->value.str);
				break;
			case TYPE_C_FUNC:
				printf("Function: \"%s\"\n", i->value.c_func->name);
				break;
			default:
				puts("ERROR - invalid type");
				break;
		}
	}
	else {
		puts("<NULL>");
	}
}
