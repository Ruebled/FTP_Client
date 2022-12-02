#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "trim.h"
#include "check.h"

//catch and disable function
//of signal SIGINT
//produced by CTRL+C
static volatile sig_atomic_t _;

static void sig_handler()
{

	printf("\b\b  \n");
	printf("ftp--> ");
	fflush(stdout);
}
//

int main(int argc, char *argv[])
{
	(void)_;

	//"disable CTRL+C signal aka SIGINT" function call
	signal(SIGINT, sig_handler);

	//char * variable for storing user input
	char *command;
	command = (char*)malloc(1000*sizeof(char));

	//char* variable for stroring server response
	char *reply;
	reply = (char*)malloc(1000*sizeof(char));

	
	while(1)
	{
		//user interface 
		//get string input from stdin
		printf("ftp--> ");
		fgets(command, 1000, stdin);
		command = trim(command);

		if(check_input_validity(command))
		{
			if(status.is_conected)
			{
				ftp_execute(command);
				char *res = ftp_response(status.socketaddr);
				process_res(res);
			}
			else
			{
				printf("Not connected\n");
			}
		}
		else
		{
			printf("Command Not Valid\n");
		}
		
	}

	free(command);
	free(reply);
	return 0;	
}
