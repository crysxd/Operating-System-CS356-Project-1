/*
 * DupShell.c
 * Author: Christian Würthner
 * Description: Simple shell application with arguments and pipes.
 */

#include "DupShell.h"

int main(int argc, char const *argv[]) {
	/* Create buffer for command */
	char command[COMMAND_MAX_LENGTH];

	/* Create buffer for the command chunks */
	char *command_parts[COMMAND_MAX_PARTS];

	/* Create buffers for single chunk strings */
	for(uint8_t i=0; i<COMMAND_MAX_PARTS; i++) {
		command_parts[i] = malloc(COMMAND_MAX_PART_LENGTH);
	}

	/* Endless loop, only stop looping when exit command is executed */
	while(1) {
		/* Print prompt */
		printf(COMMAND_PROMPT);

		/* Read command and remove line breal */
		fgets(command, COMMAND_MAX_LENGTH, stdin);
		command[strlen(command)-1] = 0;

		/* Check if the command is empty (only line break) */
		if(strlen(command) == 1) {
			continue;
		}

		/* Explode parts */
		explode_command(command, command_parts, sizeof(command_parts));

		/* Check if the command is exit */
		if(is_exit_command(command_parts)) {
			break;
		}

		/* Execute command */
		execute_command(command_parts);
	}

	return 0;
}

void explode_command(char *command, char **parts, uint16_t max_parts) {
	/* Declare counters */
	uint16_t parts_count = 0;
	uint16_t part_length = 0;
	uint8_t state = 0;
	uint16_t length = strlen(command);

	for(uint16_t i=0; i<length; i++) {
		char c = command[i];

		switch(state) {
			case 0: {
				if(c == '\\') {
					c = command[++i];
				} else {
					if(c == ' ') {
						state = 4;
						i--;
						break;
					}

					if(c == '"') {
						state = 2;
						break;
					}

					if(c == '\'') {
						state = 3;
						break;
					}

					if(c == '|') {
						state = 1;
						break;
					}
				}

				parts[parts_count][part_length++] = c;

				break;

			}

			case 1: {
				if(part_length > 0) {
					parts[parts_count++][part_length] = '\0';
				}

				parts[parts_count][0] = '|';
				parts[parts_count++][1] = '\0';
				part_length = 0;
				state = 0;
				i--;

				break;
			}

			case 2: {
				if(c == '"') {
					state = 0;
					i--;
					break;
				}

				if(c == '\\') {
					c = command[++i];
				}

				parts[parts_count][part_length++] = c;

				break;
			}

			case 3: {
				if(c == '\'') {
					state = 0;
					i--;
					break;
				}

				if(c == '\\') {
					c = command[++i];
				}

				parts[parts_count][part_length++] = c;

				break;
			}

			case 4: {
				if(c != ' ') {
					state = 0;
					i--;
					break;
				}

				if(part_length > 0) {
					parts[parts_count++][part_length] = '\0';
					part_length = 0;
				}


				break;
			}

			default: {

			}
		}
	}
	
	parts[parts_count++][part_length] = '\0';
	parts[parts_count][0] = '\0';

}

bool is_exit_command(char **parts) {
	return strcmp(parts[0], "exit") == 0;
}

void execute_command(char **parts) {
	/* Copy command parts to add a NULL at the end */
	char *parts_new[COMMAND_MAX_PARTS] = {NULL};
	uint16_t pipe_found = 0;
	int fd[2];

	/* Cancel as soon as an empty command is found */
	for(uint8_t i=0; i<COMMAND_MAX_PARTS; i++) {
		/* Empty command marks the end */
		if(strlen(parts[i]) == 0) {
			/* Error if first part is empty */
			if(i == 0) {
				printf("ERROR: Command is empty!\n");
				return;
			}
			break;
		}

		/* If the part is a pipe */
		else if(strcmp(parts[i], "|") == 0) {
			/* Error if it is the first part */
			if(i == 0) {
				printf("ERROR: First part must not be a pipe!\n");
				return ;
			}

			/* Error if there was already a pipe */
			if(pipe_found > 0) {
				printf("ERROR: Only one pipe supported!\n");
				return ;
			}

			/* Error if the next part is empty (pipe would be last) */
			if(strlen(parts[i+1]) == 0) {
				printf("ERROR: Pipe must not be last!\n");
				return ;
			}

			/* Save location and break */
			pipe_found = i;
			parts_new[i] = NULL;
		} 

		/* Default case */
		else {
			/* Copy */
			parts_new[i] = parts[i];
		}


	}

	/* Create a pipe if necessary */
	if(pipe_found > 0) {
		if(pipe(fd)) {
			printf("ERROR: Pipe creation failed!\n");
			return ;
		}
	}

	/* Fork process */
	pid_t pid = fork();

	/* Parent code */
	if(pid > 0) {
		/* Pipe was found, second child must be forked */
		if(pipe_found > 0) {

			/* Fork second child */
			pid_t pid2 = fork();

			/* Parent code, close pipe ends */
			if(pid2 > 0) {
				close(fd[0]);
				close(fd[1]);

				/* Wait for both childs to finish */
				wait(NULL);
				wait(NULL);

			}

			/* Second child, dup input and exec */
			if(pid2 == 0) {
				/* Redirect input */
				close(fd[1]);
				close(STDIN_FILENO);
				dup2(fd[0], STDIN_FILENO);
				close(fd[0]);

				/* Load new program */
				execvp(parts_new[pipe_found+1], parts_new+pipe_found+1);

				/* If this code gets executed, execlp failed */
				printf("ERROR: No such command.\n");

				/* Tell first command to terminate */
				kill(pid, SIGTERM);

				exit(1);
			}

			/* Error handling */
			if(pid2 < 0) {
				printf("ERROR: Second fork() failed.\n");

				/* Send SIGTERM to first child 1 and wait for it to exit */
				kill(pid, SIGTERM);
				wait(NULL);

			}
		} 

		/* Only one child, wait for finish */
		else {
			wait(NULL);
		}
	}

	/* Child code */
	if(pid == 0) {
		/* If a pipe is used, redirect output */
		if(pipe_found > 0) {
			close(fd[0]);
			close(STDOUT_FILENO);
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
		}

		/* Load new program */
		execvp(parts_new[0], parts_new);

		/* If this code gets executed, execlp failed */
		printf("ERROR: No such command.\n");
		exit(1);

	}

	/* Error handling */
	if(pid < 0) {
		printf("ERROR: fork() failed.\n");
	}
}
