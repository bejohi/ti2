#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "semaphore.h"

#define N_CARS 120

static const int parking_lot_size = 100;
static struct semaphore lot;
static pthread_mutex_t lock;
static int n_lots_free = parking_lot_size;

static void park(void)
{
	/* Use semaphore to prevent overusing parking lot */
	sem_wait(&lot);

	if (pthread_mutex_lock(&lock)) {
		fputs("pthread_mutex_lock() failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	n_lots_free--;
	printf("Parking lots free: %d\n", n_lots_free);
	assert(n_lots_free >= 0);

	if (pthread_mutex_unlock(&lock)) {
		fputs("pthread_mutex_unlock() failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	/* Spend random time in the parking lot */
	usleep(100000 + random()/900000);

	if (pthread_mutex_lock(&lock)) {
		fputs("pthread_mutex_lock() failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	n_lots_free++;
	printf("Parking lots free: %d\n", n_lots_free);

	if (pthread_mutex_unlock(&lock)) {
		fputs("pthread_mutex_unlock() failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	sem_signal(&lot);
}

static void drive(void)
{
	/* Spend random time on the road */
	usleep(10000 + random()/90000);
}

static void * car(void *unused)
{
	(void)unused;
	for (size_t i = 0; i < 100; i++) {
		drive();
		park();
	}

	return NULL;
}

int main(void)
{
	static pthread_t cars[N_CARS];
	sem_init(&lot, parking_lot_size);

	if (pthread_mutex_init(&lock, NULL)) {
		fputs("pthread_mutex_init() failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	for (size_t i = 0; i < N_CARS; i++) {
		if (pthread_create(&cars[i], NULL, car, NULL)) {
			fputs("pthread_create() failed\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	for (size_t i = 0; i < N_CARS; i++) {
		if (pthread_join(cars[i], NULL)) {
			fputs("pthread_join() failed\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
