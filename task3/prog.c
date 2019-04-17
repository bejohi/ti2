// Compile with: gcc -c -o prog.o -Wall -Wextra -pedantic -std=c99 prog.c*
// Test memory with: readelf -Ss prog.o

#include <stdio.h>
#include <stdlib.h>

int a = 0;
int b = 1337;

int main(void) {
	int c = 42;
	int *d;
	static int e = 0;

	if (!(d = malloc(sizeof(int)))) {
		puts("Memory allocation failed");
		exit(EXIT_FAILURE);
	}

	*d = 314159;
	printf("a = %d, b = %d, c = %d, *d = %d, e = %d\n", a, b, c, *d, e);
	return EXIT_SUCCESS;
}
