#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "simul_os.h"

static int64_t now_nanosec(void)
{
	struct timespec tp;
	if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
		perror("clock_gettime() failed");
		exit(EXIT_FAILURE);
	}
	return tp.tv_sec * 1000000000LL + tp.tv_nsec;
}

static int background_io_process(void)
{
	while (1) {
		so_io();
	}

	return 42;
}

static int background_cpu_process(void)
{
	int i = 1;
	while (i) {
		i = (i << 1) | 0x01;
		i ^= 0xaa;
	}

	return i;
}

static int foreground_process(void)
{
	for (int i = 0; i < 8; i++) {
		so_io();
	}
	return 1337;
}

int init(void)
{
	so_pid_t fg_pid;
	int64_t t_start, t_stop;

	puts("[INIT] Starting a bunch of I/O intensive background processes");
	for (int i = 0; i < 42; i++) {
		char namebuf[32];
		snprintf(namebuf, sizeof(namebuf) - 1, "bg-io-%d", i);
		if (so_create(namebuf, background_io_process, SO_PRIO_NORMAL) == -1) {
			puts("[INIT] Creating background process failed");
			exit(EXIT_FAILURE);
		}
	}

	puts("[INIT] Starting a bunch of CPU intensive background processes");
	for (int i = 0; i < 4; i++) {
		char namebuf[32];
		snprintf(namebuf, sizeof(namebuf) - 1, "bg-cpu-%d", i);
		if (so_create(namebuf, background_cpu_process, SO_PRIO_NORMAL) == -1) {
			puts("[INIT] Creating background process failed");
			exit(EXIT_FAILURE);
		}
	}

	t_start = now_nanosec();
	fg_pid = so_create("foreground", foreground_process, SO_PRIO_HIGH);
	printf("[INIT] Created foreground process with PID %i\n", (int)fg_pid);

	while (1) {
		int exit_code;
		so_pid_t pid = so_wait(&exit_code);
		if (pid == -1) {
			puts("[INIT] sg_wait() failed");
			exit(EXIT_FAILURE);
		}
		if (pid == fg_pid) {
			t_stop = now_nanosec();
			t_stop -= t_start;
			printf("[INIT] Foreground process exited with %i\n", exit_code);
			int64_t secs = t_stop / 1000000000;
			int64_t rest = t_stop % 1000000000;
			printf("[INIT] Total run time: %" PRIu64 ".%09" PRIu64 "s\n",
			       secs, rest);
			exit(EXIT_SUCCESS);
		}
	}
}
