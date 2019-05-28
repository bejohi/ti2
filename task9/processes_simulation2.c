#include <stdatomic.h>
#include <stdio.h>
#include <unistd.h>

#include "simul_os.h"

static atomic_int finished_procs = 0;

static int proc1(void)
{
	for (int i = 0; i < 3; i++) {
		so_io();
	}

	// Do atomically: finished_procs += 1
	atomic_fetch_add(&finished_procs, 1);
	return 42;
}

static int proc2(void)
{
	for (int i = 0; i < 7; i++) {
		so_io();
	}

	// Do atomically: finished_procs += 1
	atomic_fetch_add(&finished_procs, 1);
	return 0;
}

static int proc3(void)
{
	for (int i = 0; i < 5; i++) {
		so_io();
	}

	// Do atomically: finished_procs += 1
	atomic_fetch_add(&finished_procs, 1);
	return 1337;
}

static int waiting_proc(void)
{
	while (atomic_load(&finished_procs) < 3) {
		puts("[WAIT_PROC] Still waiting for proc1, proc2 and proc3");
		sleep(1);
	}

	return 314159;
}

int init(void)
{
	so_pid_t pid;

	pid = so_create("proc1", proc1);
	printf("[INIT] Created process with PID %i\n", (int)pid);

	pid = so_create("proc2", proc2);
	printf("[INIT] Created process with PID %i\n", (int)pid);

	pid = so_create("proc3", proc3);
	printf("[INIT] Created process with PID %i\n", (int)pid);

	pid = so_create("waiting_proc", waiting_proc);
	printf("[INIT] Created process with PID %i\n", (int)pid);

	for (int remaining = 4; remaining > 0; remaining--) {
		int exit_code;
		pid = so_wait(&exit_code);
		printf("[INIT] Process %i exited with %i\n", (int)pid, exit_code);
	}

	puts("[INIT] All processes have completed");
	return 0;
}
