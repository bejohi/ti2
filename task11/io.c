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
		so_lock();
		while (sched_io_q.head == NULL) {
			pthread_cond_wait(&cond_io, &core_mutex);
		}
		so_unlock();

#if ENABLE_DEBUG
		puts("[IO] I/O operation started");
#endif
		// I/O operation will take some time
		usleep(100000);
		// I/O operation of head-of-list element completed, move it to ready Q
		so_lock();
		struct so_pcb *process = sched_pop_head(&sched_io_q);
		process->state = STATE_READY;
		sched_enqueue_ready(process);
#if ENABLE_DEBUG
		puts("[IO] I/O operation completed");
		print_scheduler_state();
#endif
		so_unlock();

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
