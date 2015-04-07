/*
 * MoreShell.c
 * Author: Christian Würthner
 * Description: Simple shell application with arguments.
 */

#include "MoreShell.h"

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

		/* Read command */
		fgets(command, COMMAND_MAX_LENGTH, stdin);

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
	uint16_t part_start = 0, i = 0, parts_count = 0;

	/* Determine the length of the command */
	uint16_t command_length = strlen(command);

	/* Flag used to handle quotes */
	bool force_end_part = false;

	/* Iterate over commanf */
	while(command[i++] != 0) {
		/* If the current char is a quote, skip until the next quote */
		if(i == part_start && command[i] == '\"') {
			/* Search end */
			part_start = ++i;
			i = strchr(command+i, '\"') - command;

			/* Force end part */
			force_end_part = true;
		}

		/* If the current char is a quote, skip until the next quote */
		if(i == part_start && command[i] == '\'') {
			/* Search end */
			part_start = ++i;
			i = strchr(command+i, '\'') - command;

			/* Force end part */
			force_end_part = true;
		}

		/* If the current char is a space or we are at the end */
		if(force_end_part || command[i] == ' ' || i == command_length - 1) {
			/* Copy the current part in the parts array */
			memcpy(*(parts + parts_count), command + part_start, i - part_start);

			/* Add string terminator at the end */
			parts[parts_count][i - part_start] = 0;

			/* Skip all comming spaces (so double spaces make no problems) */
			while(command[i+1] == ' ') {
				i++;
			}

			/* Set the new part start to the next char */
			part_start = i + 1;

			/* Increment the number of parts */
			parts_count++;

			/* Reset flag */
			force_end_part = false;
		}

	}

	/* Set the net part to a empty String to identify the end */
	char c = '\0';
	memcpy(*(parts + parts_count), &c, 1);
}

bool is_exit_command(char **parts) {
	return strcmp(parts[0], "exit") == 0;
}

void execute_command(char **parts) {
	/* Fork process */
	pid_t pid = fork();

	/* Parent code */
	if(pid > 0) {
		/* Wait for child to terminate */
		wait(NULL);
	}

	/* Child code */
	if(pid == 0) {
		/* Copy command parts to add a NULL at the end */
		char *parts_new[COMMAND_MAX_PARTS] = {NULL};

		/* Cancel as soon as an empty command is found */
		for(uint8_t i=0; i<COMMAND_MAX_PARTS; i++) {
			if(strlen(parts[i]) == 0) {
				break;
			}

			/* Copy */
			parts_new[i] = parts[i];
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
