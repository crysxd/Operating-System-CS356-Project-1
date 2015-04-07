/*
 * MoreShell.h
 * Author: Christian Würthner
 * Description: Simple shell application with arguments.
 */

#define COMMAND_PROMPT "myshell> "
#define COMMAND_MAX_LENGTH 1024
#define COMMAND_MAX_PARTS 64
#define COMMAND_MAX_PART_LENGTH 128

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

void explode_command(char *command, char **parts, uint16_t max_parts);
bool is_exit_command(char **parts);
void execute_command(char **parts);