#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simul_os.h"
#include "core.h"

#define ENABLE_DEBUG                    0

struct so_pcb *sched_running = NULL;
pthread_mutex_t core_mutex = PTHREAD_MUTEX_INITIALIZER;
_Thread_local struct so_pcb *sched_me;
struct so_pcb process_table[MAX_PROCESSES];
so_pid_t last_pid = 0;

// Condition used to wait for the scheduler
static pthread_cond_t cond_sched = PTHREAD_COND_INITIALIZER;

void sched_init(void)
{
	// Nothing to do
}

void sched_wait_to_be_scheduled(void)
{
	while (sched_me->state != STATE_RUNNING) {
		pthread_cond_wait(&cond_sched, &core_mutex);
	}
}

void schedule(void)
{
	pthread_mutex_lock(&core_mutex);
	assert((!sched_running) || (sched_running == sched_me));
#if ENABLE_DEBUG
	puts("[SCHEULDER] schedule() entered");
	print_scheduler_state();
#endif
	sched_running = sched_select_next();
	if (!sched_running) {
		if (sched_are_all_queues_empty()) {
			puts("[SCHEDULER] No process in any queue ==> EXIT");
			exit(EXIT_SUCCESS);
		}
#if ENABLE_DEBUG
		puts("[SCHEDULER] No process in ready queue ==> CPU idle");
#endif
		pthread_mutex_unlock(&core_mutex);
		return;
	}

#if ENABLE_DEBUG
	printf("[SCHEDULER] Scheduled \"%s\" (PID %i)\n", sched_running->name,
	       sched_running->id);
#endif

	sched_running->state = STATE_RUNNING;
	pthread_cond_broadcast(&cond_sched);
#if ENABLE_DEBUG
	puts("[SCHEULDER] schedule() done");
	print_scheduler_state();
#endif
	pthread_mutex_unlock(&core_mutex);
}
