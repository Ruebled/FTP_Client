#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "include/ftp_data.h"
#include "include/socketfunc.h"
#include "include/check.h"
#include "include/ftpcommands.h"
#include "include/trim.h"
#include "include/misc_func.h"

#define comd_term "\r\n"
#define buff_size 4096

char* getpass(const char* prompt);

//FTP OPEN
int ftp_open(char **args) 
{
	if(!cc_status())
	{
		int port_valid = 21;

		if (strcmp(*(args+1), ""))
		{
			if(!check_ip(*(args+1)))
			{
				printf("Bad IP format\n");
				return 0;
			}
		}
		else
		{
			printf("Too few parameters\nTry help\n");
			return 0;
		}

		if (strcmp(*(args+2), ""))
		{
			if(!check_port(*(args+2)))
			{
				printf("Bad PORT format\n");
				return 0;
			}
			port_valid = atoi(*(args+2));
		}

		if (strcmp(*(args+3), ""))
		{
			printf("Too many parameters\n");
			return 0;
		}

		if(establish_control_connection(*(args+1), port_valid)<0)
		{
			return 0;
		}
		set_session_ip(*(args+1));
		return 1;						
	}
	printf("Already connected to a server\n");

	return 1;
}

//FTP USER ->Get a from username from stdin, and send via control channel
int ftp_user()
{
	char* input = (char*)malloc(sizeof(char)*32);

	printf("USER: ");
	fgets(input, 32, stdin);
	trim(input);
	
   	char* message = (char*)malloc(sizeof(char)*(strlen(input)+8));
	sprintf(message, "USER %s%s", input, comd_term);	
	free(input);

	if (control_send(get_cc_socket(), message, strlen(message))<0)
	{
		printf("Error sending the username\n");
		free(message);
		return 0;
	}

	free(message);

	char* server_reply = get_server_reply();
	//printf("%s", server_reply);
	int res = handle_response(server_reply);
	free(server_reply);
	return res;
}

//FTP PASS ->Get a from password from stdin, and send via control channel
int ftp_passwd()
{
	char* input = getpass("PASS: ");
	
	char* message = (char*)malloc(sizeof(char)*(strlen(input)+8));
	sprintf(message, "PASS %s%s", input, comd_term);	

	if (control_send(get_cc_socket(), message, strlen(message))<0)
	{
		printf("Error sending the password\n");
		free(message);
		return 0;
	}

	free(message);

	char* server_reply = get_server_reply();
	//printf("%s", server_reply);
	int res = handle_response(server_reply);
	free(server_reply);

	return res;
}
//FTP LS
int ftp_ls(char*dir)
{
	if(!cc_status())
	{
		printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
		return 0;
	}

	if(!establish_data_connection())
	{
		return 0;
	}

	char *string = (char*)malloc(sizeof(char)*(strlen(dir)+8));
	if(strcmp(dir, ""))
	{
		sprintf(string, "LIST %s%s",  dir, comd_term);
	}
	else
	{
		sprintf(string, "LIST%s", comd_term);
	}

	if (control_send(get_cc_socket(), string, strlen(string))<0)
	{
		free(string);
		printf("Error sending the LS command\n");
		return 0; 
	}
	free(string);
	
	char *reply = get_server_reply();
	printf("%s", reply);

	unsigned char *server_data = (unsigned char*)malloc(sizeof(unsigned char)*buff_size);
	memset(server_data, 0x00, buff_size);

	int dc_socket = get_dc_socket();

	do
	{
		buff_receive(dc_socket, server_data, buff_size);
		printf("%s", server_data);
		memset(server_data, 0x00, buff_size);
	}while(dc_status());

	reply = get_server_reply();
	printf("%s", reply);
	free(reply);
	free(server_data);
	return 0;
}
//FTP SYST
int ftp_syst()
{
	if(!cc_status())
	{
		printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
		return 0;
	}

	if (control_send(get_cc_socket(), "SYST\r\n", strlen("SYST\r\n"))<0)
	{
		printf("Error sending the SYST command\n");
		return 0;
	}

	char* server_reply = get_server_reply();
	//printf("%s", server_reply);
	int res = handle_response(server_reply);
	free(server_reply);
	return res;
}
//FTP RETR
int ftp_retr(char* dir)
{
	if(!cc_status())
	{
		printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
		return 0;
	}

	if(!establish_data_connection())
	{
		printf("Couldn't establish data connection\n");
		return 0;
	}

	char *message = (char*)malloc(sizeof(char)*(strlen(dir)+8));
	memset(message, 0x00, (strlen(dir)+8));
	sprintf(message, "RETR %s%s", dir, comd_term);

	ftp_type();

	control_send(get_cc_socket(), message, strlen(message));
	free(message);
	
	char* server_reply = get_server_reply();
	//printf("%s", server_reply);
	handle_response(server_reply);
	free(server_reply);

	//break if file non-existent
	if(!dc_status())
	{
		return 0;
	}

	FILE * file;
	file = fopen(dir, "wb");	
	if(file==NULL)
	{
		printf("Couldn't open file with this name\n");
		server_disconnect(get_dc_socket());
		return 0;
	}

	struct timeval stop, start;
	size_t bytes = 0;

	unsigned char *server_data = malloc(sizeof(unsigned char)*buff_size);
	memset(server_data, 0x00, buff_size);

	int dc_socket = get_dc_socket();
	int size = sizeof(unsigned char);
	int recv_size;

	gettimeofday(&start, 0);

	do
	{
		recv_size = buff_receive(dc_socket, server_data, (size*(buff_size-1)));
		fwrite(server_data, size, recv_size, file);
		memset(server_data, 0x00, buff_size);
		bytes+=recv_size;

	}while(dc_status());

	gettimeofday(&stop, 0);

	fclose(file);
	free(server_data);

	//print sending raport
	printf("%lu bytes sent in ", bytes);
	int sec = (stop.tv_sec-start.tv_sec);
	int usec = (stop.tv_usec-start.tv_usec);

	ret_time(sec, usec); 
	ret_speed(bytes, sec, usec);
	///

    server_reply = get_server_reply();
	//printf("%s", server_reply);
	int res = handle_response(server_reply);
	free(server_reply);
	return res;
}
int ftp_cwd(char* dir)
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "CWD %s%s", dir, comd_term);

		if (control_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the CWD command\n");
			free(message);
			return 0;
		}
		free(message);

		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
int ftp_test()
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "ABOR\r\n");
		printf("%s", message);


		if (control_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the CWD command\n");
			free(message);
			return 0;
		}
		free(message);

		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
int ftp_type()
{
	if(cc_status())
	{	
		if (control_send(get_cc_socket(), "TYPE I N\r\n", 10)<0)
		{
			printf("Error sending the TYPE command\n");
			return 0;
		}
		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP CDUP (Change directory up)
int ftp_cdup()
{
	if(cc_status())
	{	
		if (control_send(get_cc_socket(), "CDUP\r\n", strlen("CDUP\r\n"))<0)
		{
			printf("Error sending the CDUP command\n");
			return 0;
		}
		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP PWD(print working directory)
int ftp_pwd()
{
	if(cc_status())
	{	
		if (control_send(get_cc_socket(), "PWD\r\n", strlen("PWD\r\n"))<0)
		{
			printf("Error sending the PWD command\n");
			return 0;
		}

		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}
//FTP MKD
int ftp_mkd(char* dir)
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*(strlen(dir)+7));
		sprintf(message, "MKD %s%s", dir, comd_term);

		if (control_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("error sending the mkd command\n");
			free(message);
			return 0;
		}
		free(message);

		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("not connected to any server\ntry open [ip[port]]\n");
	return 0;
}
//FTP RMD(Remove directory)
int ftp_rmd(char* dir)
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*(strlen(dir)+7));
		sprintf(message, "RMD %s%s", dir, comd_term);

		if (control_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the RMD command\n");
			free(message);
			return 0;
		}
		free(message);

		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry open [Ip[Port]]\n");
	return 0;
}
//FTP DELE(DELETE a file)
int ftp_dele(char* file)
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*(strlen(file)+8));
		sprintf(message, "DELE %s%s", file, comd_term);

		if (control_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the DELE command\n");
			free(message);
			return 0;
		}
		free(message);

		char* server_reply = get_server_reply();
		//printf("%s", server_reply);
		int res = handle_response(server_reply);
		free(server_reply);
		return res;
	}
	printf("Not connected to any server\nTry open [Ip[Port]]\n");
	return 0;
}
//FTP STOR
int ftp_stor(char* file)
{
	if(cc_status())
	{
		FILE *ptr;

		ptr = fopen(file, "rb");
		if (ptr == NULL)
		{
			printf("Fisier neexistent\n");
			return 0;
		}

		if(!establish_data_connection())
		{
			printf("Couldn't establish data connection\n");
			return 0;
		}

		ftp_type();

		char *message = (char*)malloc(sizeof(char)*(strlen(file)+8));
		sprintf(message, "STOR %s%s", file, comd_term);

		if (control_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the STOR command\n");
			free(message);
			return 0;
		}
		free(message);

		char* reply = get_server_reply();
		printf("%s", reply);
		free(reply);

		unsigned int dc_socket;
		unsigned int size;

		size_t bytes;

		struct timeval stop, start;
		dc_socket = get_dc_socket();
		size = sizeof(unsigned char);
		unsigned char* data = malloc(sizeof(unsigned char)*buff_size);

		bytes = 0;
		gettimeofday(&start, 0);
		
		int sent_size = 0;
		do
		{
			sent_size = fread(data, size, buff_size, ptr);
			if(sent_size<buff_size&&sent_size>0)
			{
				buff_send(dc_socket, data, (size*sent_size));
				bytes+=sent_size;
				gettimeofday(&stop, 0);
				break;
			}
			else if(sent_size<0)
			{
				gettimeofday(&stop, 0);
				break;
			}
			buff_send(dc_socket, data, (size*sent_size));
			bytes+=sent_size;
		}while(1);

		//disconnect from data connection to
		//indicate completeness of sending data
		server_disconnect(dc_socket);

		fclose(ptr);
		free(data);

		get_server_reply();

		//print sending raport
		printf("%lu bytes sent in ", bytes);
		int sec = (stop.tv_sec-start.tv_sec);
		int usec = (stop.tv_usec-start.tv_usec);

		ret_time(sec, usec); 
		ret_speed(bytes, sec, usec);
		///

		return 0;
	}
	printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
	return 0;
}

//FTP QUIT
int ftp_quit()
{
	if(cc_status())
	{	
		if (control_send(get_cc_socket(), "QUIT\r\n", strlen("QUIT\r\n"))<0)
		{
			printf("Error sending the QUIT command\n");
			return 0;
		}

		if(get_server_reply()<0)
		{
			cc_disconnected();
			return -1;
		}
	}
	return -1;
}

//FTP HELP
int ftp_help()
{
	if(cc_status())
	{
		if (control_send(get_cc_socket(), "HELP\r\n", strlen("HELP\r\n"))<0)
		{
			printf("Error sending the HELP command\n");
			return 0;
		}



		int buff_reply = 2096;
		char* server_reply = (char*)malloc(sizeof(char)*buff_reply);

		control_receive(get_cc_socket(), server_reply, buff_reply);	
		//printf("%s", server_reply);
		memset(server_reply, 0x00, buff_reply);

		free(server_reply);
		return 0;
	}
	printf("open [IP] [PORT]\n"
			"*For IP - supports only IPv4"
			"\n*For PORT (optional field) for \n"
			"default port 21, write for other\n"
			"---\n"
			"Suport for connection using DNS on to do list\n"
			"Quit => close the client\n"
			);

	return 0;
}
//
//The following are the FTP commands:
//which ain't implemented
//
//            ACCT <SP> <account-information> <CRLF>
//            SMNT <SP> <pathname> <CRLF>
//            REIN <CRLF>
//            PORT <SP> <host-port> <CRLF>
//            PASV <CRLF>
//            STRU <SP> <structure-code> <CRLF>
//            MODE <SP> <mode-code> <CRLF>
//            STOU <CRLF>
//            APPE <SP> <pathname> <CRLF>
//            ALLO <SP> <decimal-integer> [<SP> R <SP> <decimal-integer>] <CRLF>
//            REST <SP> <marker> <CRLF>
//            RNFR <SP> <pathname> <CRLF>
//            RNTO <SP> <pathname> <CRLF>
//            ABOR <CRLF>
//            NLST [<SP> <pathname>] <CRLF>
//            SITE <SP> <string> <CRLF>
//            STAT [<SP> <pathname>] <CRLF>
//            noop <crlf>
