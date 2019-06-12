#include <pthread.h>
#include <stdlib.h>

#include "core.h"

int main(void)
{
	io_init();
	sched_init();
	so_create("init", init, SO_PRIO_HIGH);
	sched_algo();

	// Terminate main thread using pthread_exit() to keep other threads alive
	pthread_exit(NULL);
}
