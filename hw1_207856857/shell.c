#include "shell.h"
#include "cmds.h"

static pid_t BackgroundProcessesPid[MAX_BACKGROUND_SIZE] = {0};
static char BackgroundProcessesCmd[MAX_BACKGROUND_SIZE][1024] = {0};
static int CurrentBackgroundProcesses = 0;

char *FUNCTIONS_NAMES[] = {
    "cd",
    "jobs",
    "exit"
};
int (*FUNCTIONS_CALLBACKS[])(char **) = {
    &my_cd,
    &my_jobs,
    &my_exit
};

void handle_exec_error(const char *command) 
{
    printf("hw1shell: Failed to execute '%s': %s\n", command, strerror(errno));
}

char *read_line(void)
{
	char *line = NULL;
	size_t bufsize = 0;

    /* if getline fails */
	if (getline(&line, &bufsize, stdin) == -1) 
	{
		handle_exec_error("getline");
        printf("hw1shell: invalid command\n");
		free(line);
        return NULL;
	} 
	return (line);
}

char **split_line(char *line, int *p_num_args)
{
	int max_args_size = 64;
	int i = 0;
	char **args = malloc(max_args_size * sizeof(char *));
	char *argument;

    /* if getline failed */
    if(!line)
    {
        return NULL;
    }

	argument = strtok(line, TOK_DELIM);
	while (argument != NULL)
	{
		args[i] = argument;
		i++;
        /* this should not happen */
        if (i == max_args_size -1)
        {
            break;
        }
		argument = strtok(NULL, TOK_DELIM);
	}
	args[i] = NULL;
    *p_num_args = i;
	return (args);
}

int execute_args(char **args, int num_args)
{
	unsigned int i = 0;
    int is_in_background = 0;

	if (args[0] == NULL)
	{
		/* empty command was entered */
		return (VALID_STATUS);
	}

    /* find if the command is background */
    if(*args[num_args-1] == '&')
    {
        is_in_background = 1;
        /* check if allowed to add a background command */
        if(CurrentBackgroundProcesses == MAX_BACKGROUND_SIZE)
        {
            printf("hw1shell: too many background commands running\n");
            return (VALID_STATUS);
        }
        args[num_args-1] = NULL;
    }

    /* find if the command is my command */
	for (; i < sizeof(FUNCTIONS_NAMES) / sizeof(char *); i++)
	{
		if (strcmp(args[0], FUNCTIONS_NAMES[i]) == 0)
		{
			return ((*FUNCTIONS_CALLBACKS[i])(args));
		}
	}

	/* create a new process */
	return (new_process(args, is_in_background));
}

int new_process(char **args, int is_in_background)
{
	pid_t pid;
	int status;

	pid = fork();
	if (pid ==  0)
	{
		/* child process */
		if (execvp(args[0], args) == -1)
		{
            handle_exec_error("execvp");
            printf("hw1shell: invalid command\n");
		}
        exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
        if (is_in_background)
        {
            update_new_background_process(args, pid);
        }
        else
        {
            /* parent process */
            do 
            {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
	}
    else
	{
		handle_exec_error("fork");
        return (INVALID_STATUS);
	}
	return (VALID_STATUS);
}

void report_closed_background_processes()
{
    unsigned int i = 0;

    for(; i<MAX_BACKGROUND_SIZE;i++)
    {
        if (BackgroundProcessesPid[i] != 0 && waitpid(BackgroundProcessesPid[i], NULL, WNOHANG) != 0)
        {
            printf("hw1shell: pid %d finished\n", BackgroundProcessesPid[i]);
            CurrentBackgroundProcesses--;
            BackgroundProcessesPid[i] = 0;
            memset(BackgroundProcessesCmd[i], 0, sizeof(char)*1024);
        }
    }
}

void report_running_background_processes()
{
    unsigned int i = 0;

    for(; i<MAX_BACKGROUND_SIZE;i++)
    {
        if (BackgroundProcessesPid[i] != 0 && waitpid(BackgroundProcessesPid[i], NULL, WNOHANG) == 0)
        {
            printf("%d\t%s\n", BackgroundProcessesPid[i], BackgroundProcessesCmd[i]);
        }
    }
}

int kill_all_running_background_processes()
{
    unsigned int i = 0;
    int status = 0;
    for(; i<MAX_BACKGROUND_SIZE;i++)
    {
        if (BackgroundProcessesPid[i] != 0 && waitpid(BackgroundProcessesPid[i], NULL, WNOHANG) == 0)
        {
            status = kill(BackgroundProcessesPid[i], SIGKILL);
            if(status == -1)
            {
                handle_exec_error("kill");
                return (INVALID_STATUS);
            }

            waitpid(BackgroundProcessesPid[i], &status, 0);
            if((WIFEXITED(status) != 0) || (WTERMSIG(status) != SIGKILL))
            {
                handle_exec_error("waitpid");
                return (INVALID_STATUS);
            }

            CurrentBackgroundProcesses--;
            BackgroundProcessesPid[i] = 0;
            memset(BackgroundProcessesCmd[i], 0, sizeof(char)*1024);
        }
    }
    return (VALID_STATUS);
}

void update_new_background_process(char **args, int pid)
{
    unsigned int i = 0;
    unsigned int j = 1;

    for(; i<MAX_BACKGROUND_SIZE;i++)
    {
        if (BackgroundProcessesPid[i] == 0)
        {
            printf("hw1shell: pid %d started\n", pid);
            CurrentBackgroundProcesses++;
            BackgroundProcessesPid[i] = pid;
            strcpy(BackgroundProcessesCmd[i], args[0]);
            while (args[j] != NULL)
            {
                strcat(BackgroundProcessesCmd[i], (char *)(&" "));
                strcat(BackgroundProcessesCmd[i], args[j]);
                j+=1;
            }
            return;
        }
    }
}
