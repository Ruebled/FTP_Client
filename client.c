#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "trim.h"
#include "check.h"
#include "ftpcommands.h"
#include "reply_process.h"
#include "ftp_data.h"



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

	//char* variable for storing server response
	char *reply;
	reply = (char*)malloc(1000*sizeof(char));

	//create structure to store ftp status data
	struct server_status *status;
	status = create_server_status();
	
	while(1)
	{
		//user interface 
		//get string input from stdin
		printf("ftp--> ");
		fgets(command, 1000, stdin);
		command = trim(command);

		if(check_input_validity(command))
		{
			if(status->is_connected)
			{
				int code = ftp_execute(command);
				if (code == 0)//quit command cause exit code of function 'ftp_execute()' to be 0 
				{
					exit(0);
				}

				char *reply = ftp_response(status->socketaddr);
				process_res(reply);
			}
			else
			{
				printf("Not connected\n");
			}
		}
		else if (check_input_validity(command)==-1)
		{
			break;
		}
		else
		{
			printf("Command Not Valid\n");
		}
		
	}
	
	//dealloc ftp connection status data and dealloc status pointer
	delete_server_status(status);

	free(command);
	free(reply);
	return 0;	
}
