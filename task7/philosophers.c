#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
static pthread_mutex_t chopsticks[NUM_PHILOSOPHERS];
static pthread_t threads[NUM_PHILOSOPHERS];
static int ids[NUM_PHILOSOPHERS];

static inline size_t left(int philosopher_id)
{
	if (philosopher_id == 0) {
		return NUM_PHILOSOPHERS - 1;
	}

	return philosopher_id - 1;
}

static inline size_t right(int philosopher_id)
{
	if (philosopher_id == NUM_PHILOSOPHERS - 1) {
		return 0;
	}

	return philosopher_id + 1;
}

static int take_chopsticks(int philosopher_id)
{
	pthread_mutex_lock(&chopsticks[left(philosopher_id)]);
	/* Taking a chopstick takes some time ... */
	usleep(100000);

	if(pthread_mutex_trylock(&chopsticks[right(philosopher_id)]) != 0){
		pthread_mutex_unlock(&chopsticks[left(philosopher_id)]);
		return 0;
	}
	/* Taking a chopstick takes some time ... */
	usleep(100000);

	return 1;
}

static void return_chopsticks(int philosopher_id)
{
	pthread_mutex_unlock(&chopsticks[left(philosopher_id)]);
	pthread_mutex_unlock(&chopsticks[right(philosopher_id)]);
}

static void think(int philosopher_id)
{
	int usecs = random() % 1000000;
	usleep(usecs);
	printf("Philosopher %d: Eureka!\n", philosopher_id);
}

static void eat(int philosopher_id)
{
	usleep(100000);
	printf("Philosopher %d: Jummy!\n", philosopher_id);
}

static void * philosopher(void *p_id)
{
	int id = *(int *)p_id;
	for (int i = 0; i < 42; i++) {
		while(!take_chopsticks(id)){
			eat(id);
			return_chopsticks(id);
			think(id);
		};
		
	}

	return NULL;
}


int main(void)
{
	for (size_t i = 0; i < NUM_PHILOSOPHERS; i++) {
		pthread_mutex_init(&chopsticks[i], NULL);
		ids[i] = (int)i;
	}

	for (size_t i = 0; i < NUM_PHILOSOPHERS; i++) {
		if (pthread_create(&threads[i], NULL, philosopher, &ids[i])) {
			fputs("pthread_create() failed\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	for (size_t i = 0; i < NUM_PHILOSOPHERS; i++) {
		if (pthread_join(threads[i], NULL)) {
			fputs("pthread_join() failed\n", stderr);
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
