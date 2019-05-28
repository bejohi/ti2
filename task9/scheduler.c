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
static pthread_cond_t cond_usr1 = PTHREAD_COND_INITIALIZER;

static void usr1_handler(int unused)
{
	(void)unused;
	pthread_mutex_lock(&core_mutex);
	if (sched_running == sched_me) {
		// Ignore stray signal
		pthread_mutex_unlock(&core_mutex);
		return;
	}
	sched_me-> state = STATE_READY;
	pthread_cond_broadcast(&cond_usr1);
	sched_wait_to_be_scheduled();
	pthread_mutex_unlock(&core_mutex);
}

static void preempt_running(void)
{
	if ((!sched_running) || (sched_running == sched_me)) {
		return;
	}

	struct so_pcb *p = sched_running;
	sched_running = NULL;

	pthread_kill(p->thread, SIGUSR1);
	while (p->state != STATE_RUNNING) {
		pthread_kill(p->thread, SIGUSR1);
		pthread_cond_wait(&cond_usr1, &core_mutex);
	}
	sched_enqueue(&sched_ready_q, p);

#if ENABLE_DEBUG
	printf("[SCHEDULER] Preempted \"%s\" (PID %i)\n", p->name, (int)p->id);
#endif
}

void sched_init(void)
{
	// Setting one elesched_ment will initialize the rest of the struct with zero
	struct sigaction sa = {
		.sa_flags = 0,
		.sa_handler = usr1_handler,
	};

	if (sigaction(SIGUSR1, &sa, NULL)) {
		perror("sigaction() failed");
		exit(EXIT_FAILURE);
	}
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
#if ENABLE_DEBUG
	puts("[SCHEULDER] schedule() entered");
	print_scheduler_state();
#endif
	preempt_running();
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
