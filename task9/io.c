#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "core.h"
#include "simul_os.h"

#define ENABLE_DEBUG                    0

pthread_cond_t cond_io = PTHREAD_COND_INITIALIZER;
static pthread_t io_thread;

static void * io_thread_func(void *unused)
{
	(void) unused;
	while (1) {
		pthread_mutex_lock(&core_mutex);
		while (sched_io_q.head == NULL) {
			pthread_cond_wait(&cond_io, &core_mutex);
		}
		pthread_mutex_unlock(&core_mutex);

#if ENABLE_DEBUG
		puts("[IO] I/O operation started");
#endif
		// I/O operation will take some time
		usleep(100000);
		// I/O operation of head-of-list element completed, move it to ready Q
		pthread_mutex_lock(&core_mutex);
		struct so_pcb *process = sched_pop_head(&sched_io_q);
		process->state = STATE_READY;
		sched_enqueue(&sched_ready_q, process);
#if ENABLE_DEBUG
		puts("[IO] I/O operation completed");
		print_scheduler_state();
#endif
		pthread_mutex_unlock(&core_mutex);

		if (!sched_running) {
			// If CPU is idle, we run the scheduler to enqueue the process that
			// has become ready just now
			schedule();
		}
	}

	return NULL;
}

void io_init(void)
{
	if (pthread_create(&io_thread, NULL, io_thread_func, NULL)) {
		fputs("pthread_create() failed\n", stderr);
		exit(EXIT_FAILURE);
	}
}
