#define _DEFAULT_SOURCE // <-- clock_gettime()
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static double values[1000];
static const size_t n_values = sizeof(values)/sizeof(values[0]);

static int64_t now_nanosec(void)
{
	struct timespec tp;
	if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
		perror("clock_gettime() failed");
		exit(EXIT_FAILURE);
	}
	return tp.tv_sec * 1000000000LL + tp.tv_nsec;
}

static double complex_calculation(double input)
{
	for (size_t i = 0; i < 100000; i++) {
		input = sqrt(input * 3.0);
	}

	return input;
}


int main(void) {
	for (size_t i = 0; i < n_values; i++) {
		values[i] = (double)i;
	}

	int64_t start = now_nanosec();

	for (size_t i = 0; i < n_values; i++) {
		values[i] = complex_calculation(values[i]);
	}

	int64_t stop = now_nanosec();

	printf("Calculations took %" PRId64 "ns\n", stop - start);
	return EXIT_SUCCESS;
}
