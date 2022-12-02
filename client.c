#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "trim.h"
#include "socketfunc.h"

//catch and disable function
//of signal SIGINT
//produced by CTRL+C
static volatile sig_atomic_t keep_running = 1;

static void sig_handler()
{
	printf("\b\b  \n");
	printf("ftp--> ");
	fflush(stdout);
}



//start main
//

int main(int argc, char *argv[])
{

	//catch CTRL+C command and make it useless
	signal(SIGINT, sig_handler);

	//string for taking console input aka. commands
	char *str;

	//don't forget to free()...
	str = (char*)malloc(100*sizeof(char));
	
	
	if (argc == 2) 
	{
		if (!strcmp(argv[1],"--help"))
		{
			printf(
					"Syntax:\n"
					"open ip [port]\n"
					"send file_name\n"
				  );
		}
	}
	
	while(keep_running)
	{
		printf("ftp--> ");
		fgets(str, 100, stdin);
		str = trim(str);
		
		const char *ch = "";

		
		
		if (str != ch)
		{
			int res = try_execute(str);			
			switch (res)
			{
				case -1:
					if(status_connection(clientfd))
					{
						if(close_connection(clientfd)<0)
						{
							printf("Connection closed\n");
						}
					}
					printf("Bye\n");
					exit(0);
					break;
				case 0:
					//command invalid
					printf("?Invalid Command\n");
					break;
			}

		}

		//check res output and print for any 
		//error or success in stdout
		
	}
	
	//
	////assigning necesarry data for connection
	//server.sin_addr.s_addr = inet_addr("192.168.0.106");

	//if (connect(clientfd, (struct sockaddr *)&server, sizeof(server))<0){
	//	printf("connect error");
	//	return 1;
	//}
	//puts("Connected\n");

	//char server_reply[2000];
	//char *message;
	//char *pass;

	//message = "USER android\n";
	//pass = "android";
	////recv wait for response from server if less that 0 [-1] then operation failed
	//recv(clientfd, server_reply, 2000, 0);
	//puts(server_reply);
	////send function sends a message to server, returns -1 if failed, and more than or equal 0 if succeded
	//send(clientfd, message, strlen(message), 0);


	return 0;	
}


