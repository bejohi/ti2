#include <inttypes.h>
#include <stdio.h>
#include "rpl.h"

void rpl_print(struct item *i)
{
	if (i) {
		switch(i->type) {
			case TYPE_NUMBER:
				printf("%" PRId64 "\n", i->value.number);
				break;
			case TYPE_STRING:
				puts(i->value.str);
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
