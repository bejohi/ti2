#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#include "semaphore.h"

void sem_init(struct semaphore *target, int initial_value)
{
	assert(target && (initial_value > 0));
	target->value = initial_value;
}

void sem_wait(struct semaphore *sem)
{
	int is_negative;
	assert(sem);

	do {
		is_negative = sem->value < 0;
		/* Do not waste to much CPU, thus sleep a bit */
		usleep(10000);
	} while(is_negative);

	sem->value--;
}

void sem_signal(struct semaphore *sem)
{
	assert(sem);
	sem->value++;
}
