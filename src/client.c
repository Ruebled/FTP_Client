#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "include/trim.h"
#include "include/check.h"
#include "include/ftpcommands.h"
#include "include/ftp_data.h"

//catch and disable function
//of signal SIGINT
//produced by CTRL+C
static volatile sig_atomic_t _;

static void sig_handler()
{
	printf("\b\b  \b\b");
	fflush(stdout);
}

int main(int argc, char *argv[])
{
	(void)_;

	//"disable CTRL+C signal aka SIGINT" function call
	signal(SIGINT, sig_handler);

	//create structure to store ftp status data
	create_server_status();
	
	//char * variable for storing user input
	char *command;
	command = (char*)malloc(1024*sizeof(char));

	while(1)
	{
		//user interface 
		//get string input from stdin
		memset(command, 0x00, 1024);
		printf("ftp -> ");

		if(!fgets(command, 1024, stdin))
		{
			printf("Error on reading the input\n");
			exit(0);
		}

		//get rid of any before or after spaces or \n
		trim(command);

		if(check_command(command)==-1)
		{
			break;
		}	

	}
	destroy_server_status();
	free(command);
	printf("Closing the client...\n");
	return 0;	
}
