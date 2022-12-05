#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ftp_data.h"
#include "socketfunc.h"
#include "check.h"
#include "ftpcommands.h"

void ftp_execute(char** args)
{
	if(!is_connected())
	{
		ftp_connect(args);
	}	
	else
	{
		if(server_send(get_socket_addr(), get_string(args), strlen(get_string(args)))<0)
		{
			printf("Couldn't send\nProblem with connection maybe\n");
		}	
	}
	return;
}



int ftp_connect(char** args)
{
	if (check_command(*args)!=0)
	{
		printf("Not connected\nTry OPEN [IP] [PORT]\n");
		return 0;
	}

	if(create_socket()<0)
	{
		printf("Error creating socket address\n");
		return 0;
	}
	else
	{
		int state = server_connect(get_socket_addr(), *(args+1), atoi(*(args+2)));
		if (state<0)
		{
			printf("Error server connection\n");
			return 0;
		}
		printf("Conected\n");
		toogle_is_connected();
		return 1;
	}
}

char* ftp_response()
{
	return server_receive();
}
