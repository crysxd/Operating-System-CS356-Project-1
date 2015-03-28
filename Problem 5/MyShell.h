/*
 * MyShell.h
 * Author: Christian Würthner
 * Description: Simple shell application.
 */

#define COMMAND_PROMPT "myshell> "
#define COMMAND_MAX_LENGTH 1024

#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

bool is_exit_command(char *command);
void execute_command(char *command);