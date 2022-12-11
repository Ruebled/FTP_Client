#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#include "ftp_data.h"
#include "socketfunc.h"
#include "check.h"
#include "ftpcommands.h"
#include "trim.h"

//Try to connect to a server in control connection
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
	return get_server_reply();
}

int get_server_reply()
{
	char* sr;
	sr = control_receive();
	printf("%s",sr);

	return handle_response(sr);
}
//Try to connect via data channel
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

	int data_port = get_server_reply();

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
//Do response action to any reply server code
int handle_response(char* sr)
{
	char** reply = split_to_array(sr, " ");

	int reply_code = conv_to_num(*reply);

	if (reply_code == 229)
	{
		return fetch_data_port(sr);
	}
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

	if (reply_code == 257)
	{
		return 0;//maybe fetch the directory....
	}
	if (reply_code == 550)
	{
		return 0;
	}
	if (reply_code == 250)
	{
		return 0;
	}
	if (reply_code == 221)
	{
		cc_disconnected();
		return 1;
	}
	if (reply_code == 450)
	{
		return 0;
	}
	if (reply_code == 215)
	{
		return 0;
	}
	
	printf("Unknown return code %d\n", reply_code);
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
			return 1;						
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

//FTP USER ->Get a from username from stdin, and send via control channel
int ftp_user()
{
	char* input = (char*)malloc(sizeof(char)*50);

	printf("USER: ");
	fgets(input, 50, stdin);
	
	char* message = (char*)malloc(sizeof(char)*57);
	sprintf(message, "USER %s", input);	

	free(input);

	if (server_send(get_cc_socket(), message, strlen(message))<0)
	{
		printf("Error sending the username\n");
		free(message);
		return 0;
	}

	free(message);
	return get_server_reply();
}

//FTP PASS ->Get a from password from stdin, and send via control channel
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
		free(message);
		return 0;
	}

	free(message);
	
	return get_server_reply();
}
//FTP LS
int ftp_ls(char*dir)
{
	if(cc_status())
	{
		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}
		char *string = (char*)malloc(sizeof(char)*50);
		if(strcmp(dir, ""))
		{
			sprintf(string, "LIST %s\n",  dir);
		}
		else
		{
			sprintf(string, "LIST\n");
		}

		if (server_send(get_cc_socket(), string, strlen(string))<0)
		{
			printf("Error sending the LS command\n");
			return 0;
		}

		
		if(!get_server_reply())
		{
			return 0;
		}

		char *data;
		do
		{
			data = data_receive();
			printf("%s",data);
		}while(strcmp(data, ""));
		
		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP SYST
int ftp_syst()
{
	if(cc_status())
	{
		if (server_send(get_cc_socket(), "SYST\n", strlen("SYST\n"))<0)
		{
			printf("Error sending the SYST command\n");
			return 0;
		}
		
		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP RETR
int ftp_retr(char* dir)
{
	if(cc_status())
	{
		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}
		
		char *string = (char*)malloc(sizeof(char)*50);
		if(!strcmp(dir, ""))
		{
			while(!strcmp(string, ""))
			{
				printf(": ");
				fgets(string, 50, stdin);
				trim(string);
			}
		}
		else
		{
			strcpy(string, dir);
		}
		
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "RETR %s\n", string);


		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the RETR command\n");
			free(string);
			return 0;
		}
		free(message);

		if(!get_server_reply())
		{
			free(string);
			return 0;	
		}
		
		char *data;
		FILE * file;
		file = fopen(string, "wb");	
		//check if fopen is succesfull///
		free(string);
		do
		{
			data = data_receive();
			fputs(data, file);
		}while(strcmp(data, ""));

		fclose(file);

		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
int ftp_cwd(char* dir)
{
	if(cc_status())
	{	
		char *string = (char*)malloc(sizeof(char)*50);
		if(!strcmp(dir, ""))
		{
			while(!strcmp(string, ""))
			{
				printf(": ");
				fgets(string, 50, stdin);
				trim(string);
			}
		}
		else
		{
			strcpy(string, dir);
		}
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "CWD %s\n", string);
		printf("%s", message);

		free(string);
		//delete

		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the CWD command\n");
			free(message);
			return 0;
		}
		free(message);
		
		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP CDUP (Change directory up)
int ftp_cdup()
{
	if(cc_status())
	{	
		if (server_send(get_cc_socket(), "CDUP\n", strlen("CDUP\n"))<0)
		{
			printf("Error sending the CDUP command\n");
			return 0;
		}
		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP PWD(print working directory)
int ftp_pwd()
{
	if(cc_status())
	{	
		if (server_send(get_cc_socket(), "PWD\n", strlen("PWD\n"))<0)
		{
			printf("Error sending the PWD command\n");
			return 0;
		}

		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP MKD
int ftp_mkd(char* dir)
{
	if(cc_status())
	{	
		char *string = (char*)malloc(sizeof(char)*50);
		if(!strcmp(dir, ""))
		{
			while(!strcmp(string, ""))
			{
				printf(": ");
				fgets(string, 50, stdin);
				trim(string);
			}
		}
		else
		{
			strcpy(string, dir);
		}
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "MKD %s\n", string);
		free(string);

		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("error sending the mkd command\n");
			free(message);
			return 0;
		}
		free(message);

		return get_server_reply();
	}
	printf("not connected to any server\ntry open [ip[port]]\n");
	return 0;
}
//FTP RMD(Remove directory)
int ftp_rmd(char* dir)
{
	if(cc_status())
	{	
		char *string = (char*)malloc(sizeof(char)*50);
		if(!strcmp(dir, ""))
		{
			while(!strcmp(string, ""))
			{
				printf(": ");
				fgets(string, 50, stdin);
				trim(string);
			}
		}
		else
		{
			strcpy(string, dir);
		}
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "RMD %s\n", string);
		free(string);

		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the RMD command\n");
			free(message);
			return 0;
		}
		free(message);
		return get_server_reply();
	}
	printf("Not connected to any server\nTry open [Ip[Port]]\n");
	return 0;
}
//FTP DELE(DELETE a file)
int ftp_dele(char* file)
{
	if(cc_status())
	{	
		char *string = (char*)malloc(sizeof(char)*50);
		if(!strcmp(file, ""))
		{
			while(!strcmp(string, ""))
			{
				printf(": ");
				fgets(string, 50, stdin);
				trim(string);
			}
		}
		else
		{
			strcpy(string, file);
		}
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "DELE %s\n", string);
		free(string);
		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the DELE command\n");
			free(message);
			return 0;
		}
		free(message);
		
		return get_server_reply();
	}
	printf("Not connected to any server\nTry open [Ip[Port]]\n");
	return 0;
}
//FTP STOR
int ftp_stor(char* file)
{
	if(cc_status())
	{
		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}
		
		char *string = (char*)malloc(sizeof(char)*50);
		if(!strcmp(file, ""))
		{
			while(!strcmp(string, ""))
			{
				printf(": ");
				fgets(string, 50, stdin);
				trim(string);
			}
		}
		else
		{
			strcpy(string, file);
		}
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "STOR %s\n", string);

		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the STOR command\n");
			free(message);
			free(string);
			return 0;
		}
		free(message);

		if(!get_server_reply())
		{
			free(string);
			return 0;
		}

		FILE * file;
		file = fopen(string, "rb");	
		free(string);
		char* data = (char*)malloc(sizeof(char)*2000);
		do
		{
			fgets(data, 2000, file);
			server_send(get_dc_socket(),data, strlen(data));
		}while(strcmp((data+strlen(data)-1), ""));

		fclose(file);

		return get_server_reply();
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP QUIT
int ftp_quit()
{
	if(cc_status())
	{	
		if (server_send(get_cc_socket(), "QUIT\n", strlen("QUIT\n"))<0)
		{
			printf("Error sending the QUIT command\n");
			return 0;
		}

		char* sr;
		sr = control_receive();
		printf("%s",sr);
		if(!handle_response(sr))
		{
			return 0;
		}
		return -1;
	}
	return -1;
}


//FTP HELP
int ftp_help()
{
	if(cc_status())
	{
		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}

		if (server_send(get_cc_socket(), "HELP\n", strlen("HELP\n"))<0)
		{
			printf("Error sending the HELP command\n");
			return 0;
		}

		char* sr;
		sr = control_receive();
		printf("%s",sr);
		if(!handle_response(sr))
		{
			return 0;
		}
		char *data;
		do
		{
			data = data_receive();
			printf("%s",data);
		}while(strcmp(data+strlen(data)-1, ""));
		
		//sr = control_receive();
		//printf("%s",sr);
		//
		//if(!handle_response(sr))
		//{
		//	return 0;
		//}
		return 1;
	}
	printf("open [IP [PORT]]\n-> supports only IPv4\nQuit => close the client\n");
	return 0;
}

//Parse data port from EPSV reply
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
//The following are the FTP commands:
//which ain't implemented
//
//
//            ACCT <SP> <account-information> <CRLF>
//            SMNT <SP> <pathname> <CRLF>
//            REIN <CRLF>
//            PORT <SP> <host-port> <CRLF>
//            PASV <CRLF>
//            TYPE <SP> <type-code> <CRLF>
//            STRU <SP> <structure-code> <CRLF>
//            MODE <SP> <mode-code> <CRLF>
//            STOU <CRLF>
//            APPE <SP> <pathname> <CRLF>
//            ALLO <SP> <decimal-integer>
//                [<SP> R <SP> <decimal-integer>] <CRLF>
//            REST <SP> <marker> <CRLF>
//            RNFR <SP> <pathname> <CRLF>
//            RNTO <SP> <pathname> <CRLF>
//            ABOR <CRLF>
//            NLST [<SP> <pathname>] <CRLF>
//            SITE <SP> <string> <CRLF>
//            STAT [<SP> <pathname>] <CRLF>
//            HELP [<SP> <string>] <CRLF>
//            noop <crlf>
// 
