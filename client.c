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
	//reply = (char*)malloc(1000*sizeof(char));

	//create structure to store ftp status data
	create_server_status();
	
	while(1)
	{
		//user interface 
		//get string input from stdin
		printf("ftp--> ");
		fgets(command, 1000, stdin);

		//get rid of any before or after spaces
		trim(command);
		
		//split the string by " " character
		char** args;		
		args = split_to_array(command, " ");

		//check if the given string is a valid existing command
		//also check it's parameters based on the command
		int valid = check_input_validity(args);


		if(valid>0)
		{
			ftp_execute(args);

			reply = ftp_response();

			printf("%s", reply);

			process_res(reply);
		}
		else if (valid == -1)
		{
			break;
		}
		free(args);
	}
	
	//dealloc ftp connection status data and dealloc status pointer
	//delete_server_status(status);
	free(temp);
	//free(reply);
	return 0;	
}
