#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

enum num_attr {
	NO_MULTIPLE_OF_TWO_OR_THREE,
	MULTIPLE_OF_TWO_ONLY,
	MULTIPLE_OF_THREE_ONLY,
	MULTIPLE_OF_TWO_AND_THREE,
	NUM_ATTR_NUMOF,
};

const char *lookup_table[] = {
	[NO_MULTIPLE_OF_TWO_OR_THREE] = "",
	[MULTIPLE_OF_TWO_ONLY]        = "foo",
	[MULTIPLE_OF_THREE_ONLY]      = "bar",
	[MULTIPLE_OF_TWO_AND_THREE]   = "foobar",
};

void print_line(unsigned num, enum num_attr attr)
{
	assert((attr >= 0) && "attr out of range (too low)");
	assert((attr < NUM_ATTR_NUMOF) && "attr out of range (too high)");
	printf("%u %s\n", num, lookup_table[attr]);
}

int main(void)
{
	for (unsigned i = 0; i < 16; i++) {
		/* Code goes here */
	}
	return EXIT_SUCCESS;
}
