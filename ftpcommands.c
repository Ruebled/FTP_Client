#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ftp_data.h"
#include "socketfunc.h"
#include "check.h"
#include "ftpcommands.h"
#include "trim.h"

int establish_control_connection(char* IP, int PORT)
{
	
	if(create_cc_socket()<0)
	{
		printf("Couldn't create socket\n");
		return 0;
	}			
	if(server_connect(get_cc_socket(), IP, PORT)<0)
	{
		printf("Couln't connect to the server\n");
		return 0;
	}
	
	char *sr;
	sr = control_receive();
	printf("%s", sr);
	if(handle_response(sr)<0)
	{
		return 0;
	}
	return 1;

}

int handle_response(char* sr)
{
	char** reply = split_to_array(sr, " ");

	int reply_code = conv_to_num(*reply);

	
	if (reply_code == 220)
	{
		if(ftp_user()<0)
		{
			return 0;
		}
		return 1;		
	}

	if (reply_code == 331)
	{
		if(ftp_passwd()<0)
		{
			return 0;
		}
		return 1;
	}

	if (reply_code == 230)
	{
		cc_connected();
		printf("Connected to the server\n");
		return 0;
		//function to give the server system info
	}
	
	if (reply_code == 229)
	{
		return fetch_data_port(sr);
	}

	if (reply_code == 150)
	{
		dc_connected();

		return 1;
	}
	
	if (reply_code == 226)
	{
		dc_disconnected();
		return 0;
	}

	
	printf("Unknown return code\n");
	free(reply);
	return 1;
}

int fetch_data_port(char* sr)
{
	char** args = split_to_array(sr, "(");
	char** bars_code = split_to_array(*(args+1), "|");
	
	int data_code = conv_to_num(*(bars_code));
	
	if(data_code)
	{
		return data_code;
	}
	return 0;
}

int ftp_user()
{
	char* input = (char*)malloc(sizeof(char)*50);

	printf("USER: ");
	fgets(input, 50, stdin);
	
	char* message = (char*)malloc(sizeof(char)*56);
	sprintf(message, "USER %s", input);	

	free(input);

	if (server_send(get_cc_socket(), message, strlen(message))<0)
	{
		printf("Error sending the username\n");
		return 0;
	}

	free(message);

	char* sr;
	sr = control_receive();
	printf("%s", sr);

	if (handle_response(sr)<0)
	{
		free(sr);
		return 0;
	}
	free(sr);
	return 1;
}

int ftp_passwd()
{
	char* input = (char*)malloc(sizeof(char)*50);
	
	printf("PASS: ");
	fgets(input, 50, stdin);
	
	char* message = (char*)malloc(sizeof(char)*56);
	sprintf(message, "PASS %s", input);	

	free(input);

	if (server_send(get_cc_socket(), message, strlen(message))<0)
	{
		printf("Error sending the password\n");
		return 0;
	}

	free(message);

	char *sr;
	sr = control_receive();
	printf("%s", sr);
	if(handle_response(sr)<0)
	{
		free(sr);
		return 0;
	}
	free(sr);
	return 1;
}

//FTP OPEN
int ftp_open(char **args) {
	
	if(!cc_status())
	{
		int num_param = is_occupied(args+1);

		if (num_param == 1)
		{
			if(!check_ip(*(args+1)))
			{
				printf("Bad IP format\n");
				return 0;
			}

			char *str = (char*)malloc(sizeof(char)*50);
			printf("+-Give the port number\n|\n+-> ");
			fgets(str, 50, stdin);
			trim(str);

			int port = check_port(str);
			free(str);

			if(!port)
			{
				printf("Bad PORT format\nTry a number between 0 and 2^16\n");
				return 0;
			}

			if (establish_control_connection(*(args+1), port)<0)
			{
				return 0;
			}
			set_session_ip(*(args+1));
			return 1;
		}
		
		if (num_param == 2)
		{
			int ip_valid = check_ip(*(args+1));
			int port_valid = check_port(*(args+2));
			if(!ip_valid)
			{
				printf("Bad IP format\n");
				return 0;
			}
			if (!port_valid)
			{
				printf("Bad PORT format\nTry a number between 0 and 2^16\n");
				return 0;
			}

			if(!establish_control_connection(*(args+1), port_valid))
			{
				return 0;
			}
			set_session_ip(*(args+1));
						
		}

		if (num_param == 0)
		{
			printf("Not enought parameters\n OPEN [IP [PORT]]\n");
			return 0;
		}

		if (num_param > 2)
		{
			printf("Too many parameters\n OPEN [IP [PORT]]\n");
			return 0;
		}
	}
	printf("Already connected to a server\n");

	//maybe disconnect and try again?
	return 1;
}

int establish_data_connection()
{
	if(!create_dc_socket())
	{
		printf("Can't create data connection socket\nStrange....\n");
		return 0;
	}

	if (server_send(get_cc_socket(), "EPSV\n", strlen("EPSV\n"))<0)
	{
		printf("Error sending the EPSV command\n");
		return 0;
	}

	char* sr;
	sr = control_receive();
	printf("%s",sr);

	int data_port = handle_response(sr);

	if (!data_port)
	{
		return 0;
	}
		
	if(server_connect(get_dc_socket(), get_session_ip(), data_port)<0)
	{
		printf("Couldn't connect to data server\n");
		return 0;
	}
	dc_connected();
	
	return 1;
}
//FTP LS
int ftp_ls(char** args)
{
	if(cc_status())
	{
		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}

		if (server_send(get_cc_socket(), "LIST\n", strlen("LIST\n"))<0)
		{
			printf("Error sending the LS command\n");
			return 0;
		}

		char* sr;
		sr = control_receive();
		printf("%s",sr);
		if(!handle_response(sr))
		{
			return 0;
		}

		sr = data_receive();
		printf("%s",sr);
		
		sr = control_receive();
		printf("%s",sr);
		
		if(!handle_response(sr))
		{
			return 0;
		}

		free(sr);
		return 1;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}

//FTP RETR
int ftp_retr(char** args)
{
	if(cc_status())
	{
		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}
		
		char *string = (char*)malloc(sizeof(char)*50);
		fgets(string, 50, stdin);
		sprintf(string, "RETR %s", string);

		if (server_send(get_cc_socket(), string, strlen(string))<0)
		{
			printf("Error sending the LS command\n");
			return 0;
		}

		char* sr;
		sr = control_receive();
		printf("%s",sr);
		if(!handle_response(sr))
		{
			return 0;
		}

		FILE * file;
		file = fopen("logs", "w");	
		do
		{
			sr = data_receive();
			fprintf(file, "%s",sr);
		}while(strcmp(sr+strlen(sr), ""));

		fclose(file);

		sr = control_receive();
		printf("%s\n",sr);

		if(!handle_response(sr))
		{
			return 0;
		}

		
		free(sr);
		return 1;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}

//FTP QUIT
int ftp_quit()
{

	printf("Quit\n");
	return -1;
}
//The following are the FTP commands:
//
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
//            NLST [<SP> <pathname>] <CRLF>
//            SITE <SP> <string> <CRLF>
//            SYST <CRLF>
//            STAT [<SP> <pathname>] <CRLF>
//            HELP [<SP> <string>] <CRLF>
//            noop <crlf>
// 
