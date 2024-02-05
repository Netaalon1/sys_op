#include "cmds.h"


int my_cd(char **args)
{
    /* no input */
	if (args[1] == NULL)
    {
        return (INVALID_STATUS);
	}
    /* chdir failed */
    if (chdir(args[1]) != 0)
    {
        handle_exec_error("chdir");
        return (INVALID_STATUS);
    }
	return (VALID_STATUS);
}


int my_jobs(char **args)
{
    if(args[1] != NULL)
    {
        return (INVALID_STATUS);
    }
    report_running_background_processes();
	return (VALID_STATUS);
}

int my_exit(char **args)
{
    kill_all_running_background_processes();
	/* exit with status */
	if (args[1])
	{
	    exit(atoi(args[1]));
	}
	/* exit success */
	else
	{
		exit(1);
	}
    return (1);
}
