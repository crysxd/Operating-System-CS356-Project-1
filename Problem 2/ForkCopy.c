/*
 * ForkCopy.c
 * Author: Christian Würthner
 * Description: Use MyCopy to copy a file in a new process.
 */

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[]) {
	
	/* Fork process */
	pid_t pid = fork();

	/* Code for parent */
	if(pid != 0) {
		/* Wait for child */
		int status = 0;
		wait(&status);

		/* Query the plain exit status */
		status = WEXITSTATUS(status);

		/* Print success or error text */
		if(status != 0) {
			printf("ERROR: child process finished abnormally with status %d\n", status);
		} else {
			printf("SUCCESS: Child finished normally.\n");
		}

		/* Return the child's exit status */
		return status;

	}

	/* Code for child */
	else if(pid == 0) {
		execlp("./MyCopy", argv[0], argv[1], argv[2], NULL);
	}

	/* Code for error handling */
	else if(pid < 0) {
		printf("ERROR: Unable to for process!\n");
		return 1;
	}

	/* Print success and exit */
	printf("SUCCESS.\n");
	return 0;
}