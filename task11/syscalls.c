#include <assert.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simul_os.h"
#include "core.h"

#define ENABLE_DEBUG                    0

static pthread_cond_t cond_exit = PTHREAD_COND_INITIALIZER;

static void *process_wrapper(void *_pcb)
{
	struct so_pcb *pcb = _pcb;
	so_lock();
	sched_me = pcb;
	sched_wait_to_be_scheduled();
	so_unlock();

	int exit_code = pcb->program();

	so_lock();
	pcb->exit_code = exit_code;
	pcb->state = STATE_ZOMBIE;
	sched_running = NULL;
	if (pcb->parent) {
		struct so_pcb *end = pcb->parent;

		// Traverse to end of list
		while (end->dead_childs != NULL) {
			end = end->dead_childs;
		}

		end->dead_childs = pcb;
	}

	pthread_cond_broadcast(&cond_exit);
	so_unlock();
	printf("[SIMUL-OS] Process \"%s\" (PID %i) exited\n",
	       pcb->name, (int)pcb->id);
	schedule();
	return NULL;
}

so_pid_t so_create(const char *name, int (*program)(void), enum so_prio prio)
{
	so_lock();

	if ((last_pid >= MAX_PROCESSES) || !name || !program) {
		so_unlock();
		return -1;
	}

	struct so_pcb *pcb = &process_table[last_pid++];
	pcb->id = last_pid;
	pcb->program = program;
	pcb->parent = sched_me;
	strncpy(pcb->name, name, sizeof(pcb->name));
	pcb->name[sizeof(pcb->name) - 1] = '\0';
	pcb->state = STATE_READY;
	pcb->prio = prio;
	sched_enqueue_ready(pcb);
	so_unlock();

	if (pthread_create(&pcb->thread, NULL, process_wrapper, pcb)) {
		fputs("pthread_create() failed\n", stderr);
		exit(EXIT_FAILURE);
	}

	return pcb->id; // <-- pid is never changed, no need to lock mutex
}

so_pid_t so_getpid(void)
{
	return sched_me->id; // <-- pid is never changed, no need to lock mutex
}

static void block(void)
{
	assert((sched_running != NULL) && (sched_me == sched_running));
	sched_me->state = STATE_BLOCKED;
	sched_running = NULL;
}

so_pid_t so_wait(int *code)
{
	so_lock();
#if ENABLE_DEBUG
	printf("[SYSCALL] so_wait() called by \"%s\" (PID %i)\n",
	       sched_me->name, (int)sched_me->id);
	print_scheduler_state();
#endif
	if (sched_are_all_queues_empty()) {
		// No other process there to wait for
		so_unlock();
		return -1;
	}

	block();
	sched_enqueue_blocked(sched_me);
	so_unlock();

	schedule();

	so_lock();
	while (!sched_me->dead_childs) {
		pthread_cond_wait(&cond_exit, &core_mutex);
	}

	sched_remove_from_queue(&sched_blocked_q, sched_me);
	sched_enqueue_ready(sched_me);
	sched_me->state = STATE_READY;

	so_pid_t retval = sched_me->dead_childs->id;
	if (code) {
		*code = sched_me->dead_childs->exit_code;
	}
	sched_me->dead_childs->state = STATE_EXIT;
	sched_me->dead_childs = sched_me->dead_childs->dead_childs;

	if (!sched_running) {
		so_unlock();
		schedule();
		so_lock();
	}
	sched_wait_to_be_scheduled();
#if ENABLE_DEBUG
	printf("[SYSCALL] so_wait() completed for \"%s\" (PID %i)\n",
	       sched_me->name, (int)sched_me->id);
	print_scheduler_state();
#endif
	so_unlock();
	return retval;
}

int so_io(void)
{
	so_lock();
#if ENABLE_DEBUG
	printf("[SYSCALL] so_io() called by \"%s\" (PID %i)\n",
	       sched_me->name, (int)sched_me->id);
	print_scheduler_state();
#endif
	block();
	sched_enqueue_io(sched_me);
	pthread_cond_signal(&cond_io);
	so_unlock();
	schedule();
	so_lock();
	sched_wait_to_be_scheduled();
#if ENABLE_DEBUG
	printf("[SYSCALL] so_io() completed for \"%s\" (PID %i)\n",
	       sched_me->name, (int)sched_me->id);
	print_scheduler_state();
#endif
	so_unlock();
	return 0;
}

void so_yield(void)
{
	so_lock();
#if ENABLE_DEBUG
	printf("[SYSCALL] so_yield() called by \"%s\" (PID %i)\n",
	       sched_me->name, (int)sched_me->id);
	print_scheduler_state();
#endif
	if (sched_running == sched_me) {
		sched_running = NULL;
		sched_me->state = STATE_READY;
		sched_enqueue_ready(sched_me);
		so_unlock();
		schedule();
		so_lock();
	}
	sched_wait_to_be_scheduled();
#if ENABLE_DEBUG
	printf("[SYSCALL] so_yield() completed for \"%s\" (PID %i)\n",
	       sched_me->name, (int)sched_me->id);
	print_scheduler_state();
#endif
	so_unlock();
}
