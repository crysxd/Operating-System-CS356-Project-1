/*
 * PipeCopy.c
 * Author: Christian Würthner
 * Description: Use two processes communicating with a pipe to copy a file
 */

#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

/* Size of the data blocks copied in bytes */
#define BLOCK_SIZE 1024

void print_done(pid_t child_1, pid_t done, int status) {
	/* Check which child is done */
	uint8_t child_number = 0;
	if(child_1 == done)
		child_number = 1;
	else
		child_number = 2;

	/* Query the plain exit status */
	status = WEXITSTATUS(status);

	/* Print success message */
	if(status == 0) {
		printf("SUCCESS: Child %d finished normally.\n", child_number);
	}

	/* Print error message */
	else {
		printf("ERROR: Child %d finished abnormally with status %d\n", child_number, status);

	}
}

void copy(int src, int dest, int thread_id) {
	/* Create buffer and counter */
	uint8_t buffer[BLOCK_SIZE];
	int16_t read_count = 0;

	/* Copy blocks */
	while((read_count = read(src, buffer, BLOCK_SIZE)) > 0) {
		printf("[%d] %d bytes copied...\n", thread_id, read_count);
		write(dest, buffer, read_count);
	}
}

int main(int argc, char const *argv[]) {
	/* Create ordinary pipe */
	int fd[2];
	if (pipe(fd)) {
		fprintf (stderr, "ERROR: Pipe creation failed.\n");
		return 1;
	}

	/* Fork process */
	pid_t child_1 = fork();
	pid_t child_2 = 0;

	/* Code for parent after forking child 1 */
	if(child_1 > 0) {
		/* Fork process again */
		child_2 = fork();
	}

	/* Code for parent after forking child 2 */
	if(child_2 > 0) {
		/* Close both ends of the pipe for parent */
		close(fd[0]);
		close(fd[1]);

		/* Wait for both childs to finish */
		for(uint8_t i=0; i<2; i++) {
			int status;
			pid_t done = wait(&status);
			print_done(child_1, done, status);
		}

		/* Print message that all childs are terminated */
		printf("SUCCESS: All children are terminated.\n");
		return 0;
	}

	/* Code for child 1 (read file) */
	if(child_1 == 0) {
		/* Close read end of pipe */
		close(fd[0]);

		/* Open file pointer for source and handle error */
		int src = open(argv[1], O_RDONLY);
		if(src < 0) {
			printf("ERROR: Unable to open source file \"%s\" (%s)\n", argv[1], strerror(errno));	
			close(fd[1]);		
			return 1;
		}		

		/* Copy from file to pipe */
		copy(src, fd[1], 1);

		/* Close file and pipe */
		close(src);
		close(fd[1]);

		return 0;
	}

	/* Code for child 2 (write file) */
	if(child_2 == 0) {
		/* Close write end of pipe */
		close(fd[1]);

		/* Open file pointer for destination and handle error */
		int dest = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
		if(dest < 0) {
			printf("ERROR: Unable to open destination file \"%s\" (%d: %s)\n", argv[2], errno, strerror(errno));	
			close(fd[0]);
			return 1;
		}	

		/* Copy from pipe to file */
		copy(fd[0], dest, 2);

		/* Close file and pipe */
		close(dest);
		close(fd[0]);

		return 0;
	}

	/* Code for error handling */
	if(child_1 < 0 || child_2 < 0) {
		printf("ERROR: Unable to for process!\n");
		return 1;
	}

	return 0;
}