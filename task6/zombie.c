#define _DEFAULT_SOURCE // <-- For usleep()

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
	pid_t parent = getpid();
	pid_t child = fork();

	if (child == -1) {
		perror("Failed to fork");
		exit(EXIT_FAILURE);
	}

	if (child == 0) {
		static const int exit_status = 42;
		printf("I'm the child and my exit status is %d\n", exit_status);
		exit(exit_status);
	}
	else {
		int status;
		/* sleep for 100ms so that the child's message gets printed first */
		usleep(100000);
		printf("Child process has id %d, and parent process has id %d\n",
		       (int)child, (int)parent);
		printf("Run \"ps aux\" now and check for processes %d and %d\n",
		       (int)child, (int)parent);
		printf("Can you terminate the child process with \"kill %d\"?\n",
		       (int)child);
		puts("Press any key to continue...");
		getchar();

		wait(&status);
		if (WIFEXITED(status)) {
			printf("Child exited with status %d\n", WEXITSTATUS(status));
		}
		else {
			puts("Something unexpected happened...");
		}

		printf("Run \"ps aux\" now and check again for processes %d and %d\n",
		       (int)child, (int)parent);
		puts("Press any key to continue...");
		getchar();
	}

	puts("Check \"ps aux\" one last time!");
	return EXIT_SUCCESS;
}
