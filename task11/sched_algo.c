#include "core.h"

struct so_pcb *sched_select_next(void)
{
	// Algorithm: FIFO. Simply get the first ready process
	return sched_pop_head(&sched_ready_q);
}

void sched_algo(void)
{
	// Algorithm: FIFO. Just call schedule once
	// schedule() will be called again when a process blocks or exits
	schedule();
}

