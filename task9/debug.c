#include <assert.h>
#include <stdio.h>

#include "core.h"

static void print_queue(struct sched_q *queue)
{
	assert(queue);
	struct so_pcb *p = queue->head;
	if (p) {
		printf("\"%s\" (PID %i)", p->name, (int)p->id);
		for (p = p->next; p != NULL; p = p->next) {
			printf(", \"%s\" (PID %i)", p->name, (int)p->id);
		}
		puts("");
	}
	else {
		puts("<EMPTY>");
	}
}

void print_scheduler_state(void)
{
	fputs("[DEBUG] Running:       ", stdout);
	if (sched_running) {
		printf("\"%s\" (PID %i)\n",
		       sched_running->name, (int)sched_running->id);
	}
	else {
		puts("<NONE>");
	}

	fputs("        Ready:         ", stdout);
	print_queue(&sched_ready_q);

	fputs("        Blocked:       ", stdout);
	print_queue(&sched_blocked_q);

	fputs("        Blocked (I/O): ", stdout);
	print_queue(&sched_io_q);
}
