#include "shell.h"

int main(void)
{
	char *line;
	char **args;
	int status = VALID_STATUS;

	while (status == VALID_STATUS) 
	{
		int num_args;

		report_closed_background_processes();
		printf("hw1shell$ "); 
		line = read_line(); 
		args = split_line(line, &num_args); 
		status = execute_args(args, num_args);
		
		free(line);
		free(args);
		
		if(status == INVALID_STATUS)
		{
			printf("hw1shell: invalid command\n");
			status = VALID_STATUS;

		}
	};
}
