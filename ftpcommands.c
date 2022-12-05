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
int ftp_ls(char** args)
{
	if(is_connected())
	{
		
		if (server_send(get_socket_addr(), "EPSV\n", strlen("EPSV\n"))<0)
		{
			printf("Error sending the LS command\n");
			return 0;
		}
		char* sr;
		sr = server_receive();
		printf("%s",sr);

		create_server_data();
		create_data_socket();

		char*stu = malloc(sizeof(char)*10);
		fgets(stu, 10, stdin);
		trim(stu);	
		if(server_connect(get_data_addr(), "192.168.0.106", conv_to_num(stu))<0)
		{
			printf("Couldn't connect to data server\n");

		}
		if (server_send(get_socket_addr(), "LIST\n", strlen("LIST\n"))<0)
		{
			printf("Error sending the LS command\n");
			return 0;
		}
		//char* sr;
		sr = server_data_receive();
		printf("%s",sr);
		free(sr);
		return 1;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}

//FTP QUIT

//The following are the FTP commands:
//
//            USER <SP> <username> <CRLF>
//            PASS <SP> <password> <CRLF>
//            ACCT <SP> <account-information> <CRLF>
//            CWD  <SP> <pathname> <CRLF>
//            CDUP <CRLF>
//            SMNT <SP> <pathname> <CRLF>
//            QUIT <CRLF>
//            REIN <CRLF>
//            PORT <SP> <host-port> <CRLF>
//            PASV <CRLF>
//            TYPE <SP> <type-code> <CRLF>
//            STRU <SP> <structure-code> <CRLF>
//            MODE <SP> <mode-code> <CRLF>
//            RETR <SP> <pathname> <CRLF>
//            STOR <SP> <pathname> <CRLF>
//            STOU <CRLF>
//            APPE <SP> <pathname> <CRLF>
//            ALLO <SP> <decimal-integer>
//                [<SP> R <SP> <decimal-integer>] <CRLF>
//            REST <SP> <marker> <CRLF>
//            RNFR <SP> <pathname> <CRLF>
//            RNTO <SP> <pathname> <CRLF>
//            ABOR <CRLF>
//            DELE <SP> <pathname> <CRLF>
//            RMD  <SP> <pathname> <CRLF>
//            MKD  <SP> <pathname> <CRLF>
//            PWD  <CRLF>
//            LIST [<SP> <pathname>] <CRLF>
//            NLST [<SP> <pathname>] <CRLF>
//            SITE <SP> <string> <CRLF>
//            SYST <CRLF>
//            STAT [<SP> <pathname>] <CRLF>
//            HELP [<SP> <string>] <CRLF>
//            noop <crlf>
// 
