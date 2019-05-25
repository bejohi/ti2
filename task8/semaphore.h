#ifndef SEMAPHORE_H
#define SEMAPHORE_H

struct semaphore {
	int value;
	/* Extend as needed */
};

void sem_init(struct semaphore *target, int initial_value);
void sem_wait(struct semaphore *sem);
void sem_signal(struct semaphore *sem);

#endif
