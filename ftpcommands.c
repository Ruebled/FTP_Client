#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ftp_data.h"
#include "socketfunc.h"
#include "check.h"
#include "ftpcommands.h"
#include "trim.h"

//FTP OPEN
int ftp_open(char **args) {
	
	if(!is_connected())
	{
		char *str = (char*)malloc(sizeof(char)*50);
		int num_param = is_occupied(args+1);
		if (num_param == 1)
		{
			if(check_ip(*(args+1)))
			{
				printf("+-Give the port number\n|\n+-> ");
				fgets(str, 50, stdin);
				trim(str);
				int port = check_port(str);
				if(port)
				{
					if(create_socket()<0)
					{
						printf("Couldn't create socket\n");
						return 0;
					}			
					if(server_connect(get_socket_addr(), *(args+1), port)<0)
					{
						printf("Couln't connect to the server\n");
						return 0;
					}
					
					char *sr;
					sr = server_receive();
					printf("%s", sr);
					if (!strcmp(strtok(sr, " "), "220"))
					{
						printf("USER: ");
						fgets(str, 50, stdin);
						sprintf(sr, "USER %s", str);	
						if (server_send(get_socket_addr(), sr, strlen(sr))<0)
						{
							printf("Error sending the username\n");
							return 0;
						}
						sr = server_receive();
						printf("%s", sr);
						if (!strcmp(strtok(sr, " "), "331"))
						{
							printf("PASS: ");
							fgets(str, 50, stdin);
							sprintf(sr, "PASS %s", str);
							if (server_send(get_socket_addr(), sr, strlen(sr))<0)
							{
								printf("Error sending the password\n");
								return 0;
							}
							sr = server_receive();
							printf("%s", sr);
							if(!strcmp(strtok(sr, " "), "230"))
							{
								toogle_is_connected();
								printf("Connected\n");
								return(0);
							}
							return 0;
						}
					}	
				}
				printf("Bad PORT format\nTry a number between 0 and 2^16\n");
				return 0;
			}
			printf("Bad IP format\n");
			return 0;
		}
		if (num_param == 2)
		{
			
			int port = check_port(*(args+2));
			if(check_ip(*(args+1)) && port)
			{
				if(create_socket()<0)
				{
					printf("Couldn't create socket\n");
					return 0;
				}			
				if(server_connect(get_socket_addr(), *(args+1), port)<0)
				{
					printf("Couln't connect to the server\n");
					return 0;
				}

				
				char *sr;
				sr = server_receive();
				printf("%s", sr);
				if (!strcmp(strtok(sr, " "), "220"))
				{
					printf("USER: ");
					fgets(str, 50, stdin);
					sprintf(sr, "USER %s", str);	
					if (server_send(get_socket_addr(), sr, strlen(sr))<0)
					{
						printf("Error sending the username\n");
						return 0;
					}
					sr = server_receive();
					printf("%s", sr);
					if (strcmp(strtok(sr, " "), "331"))
					{
						printf("PASS: ");
						fgets(str, 50, stdin);
						sprintf(sr, "PASS %s", str);
						if (server_send(get_socket_addr(), sr, strlen(sr))<0)
						{
							printf("Error sending the password\n");
							return 0;
						}
						sr = server_receive();
						printf("%s", sr);
						if(!strcmp(strtok(sr, " "), "230"))
						{
							toogle_is_connected();
							printf("Connected\n");
							return(0);
						}
						return 0;
					}
				}	
			}
			return 0;
		}
		free(str);
		if (num_param == 0)
		{
			printf("Not enought parameters\n OPEN [IP[PORT]]\n");
			return 0;
		}
		if (num_param > 2)
		{
			printf("Too many parameters\n OPEN [IP[PORT]]\n");
			return 0;
		}
	}
	printf("Already connected to a server\n");
	return 1;
}

//FTP LS
int ftp_ls(char* dir_name)
{
	if(is_connected())
	{
		if (server_send(get_socket_addr(), "LS\n", strlen("LS\n"))<0)
		{
			printf("Error sending the LS command\n");
			return 0;
		}
		char* sr;
		sr = server_receive();
		while(sr!=NULL)
		{
			printf("%s",sr);
			sr = server_receive();
		}
		return 1;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}

//FTP 
