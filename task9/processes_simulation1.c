#include <stdio.h>

#include "simul_os.h"

static int proc1(void)
{
	for (int i = 0; i < 3; i++) {
		so_io();
	}
	return 42;
}

static int proc2(void)
{
	for (int i = 0; i < 7; i++) {
		so_io();
	}
	return 0;
}

static int proc3(void)
{
	for (int i = 0; i < 5; i++) {
		so_io();
	}
	return 1337;
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

	for (int remaining = 3; remaining > 0; remaining--) {
		int exit_code;
		pid = so_wait(&exit_code);
		printf("[INIT] Process %i exited with %i\n", (int)pid, exit_code);
	}

	puts("[INIT] All processes have completed");
	return 0;
}
