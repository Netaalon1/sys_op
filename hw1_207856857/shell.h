#ifndef SHELL_H
#define SHELL_H

/*---LIBRARIES---*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>


#define VALID_STATUS        -1
#define INVALID_STATUS      0
#define TOK_DELIM           " \t\r\n\a\""
#define MAX_BACKGROUND_SIZE 4


void handle_exec_error(const char *command) ;
char *read_line(void);
char **split_line(char *line, int *p_num_args);
int execute_args(char **args, int num_args);
int new_process(char **args, int is_in_background);
void report_closed_background_processes();
void report_running_background_processes();
int kill_all_running_background_processes();
void update_new_background_process(char **args, int pid);
#endif