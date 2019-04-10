#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char global_msg[] = "Hallo Welt!\n";

int main(void)
{
	size_t msg_len = strlen(global_msg);
	char *msg;
	size_t pos = 0;

	if (!(msg = malloc(msg_len))) {
		exit(EXIT_FAILURE);
	}

	memcpy(msg, global_msg, msg_len);

	while (msg_len) {
		ssize_t retval;
		while (-1 == (retval = write(STDOUT_FILENO, msg + pos, msg_len))) {
			if (errno == EINTR) {
				continue;
			}

			perror("write");
			exit(EXIT_FAILURE);
		}

		pos += (size_t)retval;
		msg_len -= (size_t)retval;
	}

	return EXIT_SUCCESS;
}
