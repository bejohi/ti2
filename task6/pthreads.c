// Kompilieren mit gcc pthreads.c -o pthreads -O3 -Wall -Wextra -Werror -pedantic -std=c99 -lm -pthread
#define _DEFAULT_SOURCE // <-- clock_gettime()
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

static double values[1000];
static const size_t n_values = sizeof(values)/sizeof(values[0]);

struct t_complex_data {
	pthread_t* tids;
	double* data;
} typedef t_complex_data;

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

void *thread_calc(void* arr){
	// TODO: Repair index problems
	pthread_t tid = pthread_self();
	int startIndex = (int) tid * 255;
	int endIndx = startIndex + 255;
	printf("%ld|%d|%d\n",tid,startIndex,endIndx);
	if(endIndx > 1000){
		return NULL;
	}
	double* values_arr = (double*) arr;
	for(int i = startIndex; i < endIndx;i++){
		values_arr[i] = complex_calculation(values_arr[i]);
	}
	return NULL;
}

int main(void) {
	for (size_t i = 0; i < n_values; i++) {
		values[i] = (double)i;
	}

	pthread_t* p1, p2, p3, p4;
	

	int64_t start = now_nanosec();
	pthread_create(&p1, NULL, thread_calc, &values);
	pthread_create(&p2, NULL, thread_calc, &values);
	pthread_create(&p3, NULL, thread_calc, &values);
	pthread_create(&p4, NULL, thread_calc, &values);

	pthread_exit(NULL);

	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	pthread_join(p3,NULL);
	pthread_join(p4,NULL);
	int64_t stop = now_nanosec();

	printf("Calculations took %" PRId64 "ns\n", stop - start);
	return EXIT_SUCCESS;
}
