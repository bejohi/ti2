#include <assert.h>
#include "core.h"

struct sched_q sched_ready_q = {.head = NULL, .tail = NULL};
struct sched_q sched_blocked_q = {.head = NULL, .tail = NULL};
struct sched_q sched_io_q = {.head = NULL, .tail = NULL};

int sched_are_all_queues_empty(void)
{
	return ((sched_ready_q.head == NULL) &&
	        (sched_blocked_q.head == NULL) &&
	        (sched_io_q.head == NULL));
}

struct so_pcb *sched_pop_tail(struct sched_q *queue)
{
	assert(queue);
	if (queue->tail == NULL) {
		return NULL;
	}

	struct so_pcb *retval = queue->tail;
	queue->tail = retval->prev;
	if (queue->tail) {
		queue->tail->next = NULL;
	}
	else {
		queue->head = NULL;
	}

	retval->prev = NULL;
	return retval;
}

struct so_pcb *sched_pop_head(struct sched_q *queue)
{
	assert(queue);
	if (queue->head == NULL) {
		return NULL;
	}

	struct so_pcb *retval = queue->head;
	queue->head = retval->next;
	if (queue->head) {
		queue->head->prev = NULL;
	}
	else {
		queue->tail = NULL;
	}

	retval->next = NULL;
	return retval;
}

static void sched_enqueue(struct sched_q *queue, struct so_pcb *process)
{
	assert(queue && process);
	if (!queue->tail) {
		queue->tail = process;
		queue->head = process;
		process->prev = NULL;
		process->next = NULL;
	}
	else {
		process->next = NULL;
		process->prev = queue->tail;
		queue->tail->next = process;
		queue->tail = process;
	}
}

void sched_enqueue_blocked(struct so_pcb *process)
{
	sched_enqueue(&sched_blocked_q, process);
}

void sched_enqueue_io(struct so_pcb *process)
{
	sched_enqueue(&sched_io_q, process);
}

void sched_enqueue_ready(struct so_pcb *process)
{
	sched_enqueue(&sched_ready_q, process);
}

void sched_remove_from_queue(struct sched_q *queue, struct so_pcb *process)
{
	assert(queue && process);
	// Assert that the process is indeed enqueued in queue (for some cases)
	assert((queue->head != queue->tail) || (queue->head == process));

	if (queue->head == queue->tail) {
		// Removed only remaining process from queue
		queue->head = queue->tail = NULL;
	}
	else {
		process->prev->next = process->next;
		process->next->prev = process->prev;
		process->prev = process->next = NULL;
	}
}
