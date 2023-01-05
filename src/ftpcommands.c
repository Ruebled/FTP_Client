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

char *getpass(const char *prompt);//get password without echoing

//Try to connect to a server in control connection
int establish_control_connection(char* IP, int PORT)
{
	if(create_cc_socket()<0)
	{
		printf("Couldn't create socket\n");
		return -1;
	}			
	if(server_connect(get_cc_socket(), IP, PORT)<0)
	{
		printf("Couln't connect to the server\n");
		return -1;
	}
	cc_connected();
	return get_server_reply();
}

int get_server_reply()
{
	char* server_reply = malloc(sizeof(char)*401);
	control_receive(server_reply);
	printf("%s",server_reply);

	int res = handle_response(server_reply);

	memset(server_reply, 0x00, 401);
	free(server_reply);
	server_reply = NULL;
	return res;
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
	char** reply = split_to_array(sr, " ", 1);

	int reply_code = conv_to_num(*reply);

	destroy(reply, 1);

	switch(reply_code)
	{
		case 257:
		case 250:
		case 200:
		case 215:
		case 502:
		case 221:
		case 150:
		case 214:
			return 0;

		case 226:
		case 450:
		case 550:
			dc_disconnected();
			return 0;
		case 220:
			return ftp_user();

		case 229:
			return fetch_data_port(sr);

		case 331:
			return ftp_passwd();

		case 230:
			printf("Connected to the server\n");
			return 0;

		case 332:
			printf("ACCT info required\nNot yet created the function\n");
			return 0;
		case 530:
			cc_disconnected();
			return 0;

		default:
			printf("Unknown return code %d\n", reply_code);
			return 0;
	}
}

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
	char* input = getpass("PASS: ");
	
	char* message = (char*)malloc(sizeof(input)+6);
	sprintf(message, "PASS %s\n", input);	

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
	if(!cc_status())
	{
		printf("Not connected to any server\nTry OPEN [IP[PORT]]\n");
		return 0;
	}

	if(!establish_data_connection())
	{
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
		free(string);
		printf("Error sending the LS command\n");
		return 0;
	}
	free(string);
	
	get_server_reply();

	if(dc_status())
	{
		unsigned char *server_data = malloc(sizeof(unsigned char));
		do
		{
			info_receive(server_data);
			printf("%c", *server_data);

		}while(dc_status());

		free(server_data);

		return get_server_reply();
	}
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

	if (server_send(get_cc_socket(), "SYST\n", strlen("SYST\n"))<0)
	{
		printf("Error sending the SYST command\n");
		return 0;
	}

	return get_server_reply();
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

	char *message = (char*)malloc(sizeof(char)*50);
	sprintf(message, "RETR %s\n", dir);

	ftp_type();

	if (server_send(get_cc_socket(), message, strlen(message))<0)
	{
		printf("Error sending the RETR command\n");
		server_disconnect(get_dc_socket());
		return 0;
	}
	free(message);
	get_server_reply();

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
	int size = sizeof(unsigned char);

	///compatibility between char* and unsigned char*
	unsigned char *server_data = malloc(sizeof(unsigned char));
	gettimeofday(&start, 0);
	do
	{
		data_receive(server_data);
		fwrite(server_data, size, 1, file);
		bytes++;

	}while(dc_status());

	gettimeofday(&stop, 0);

	fclose(file);
	free(server_data);

	//print received raport
	printf("%lu bytes received in ", bytes);
	ret_time((stop.tv_sec-start.tv_sec), (stop.tv_usec-start.tv_usec)/(int)(1000)); 
	ret_speed(bytes, (stop.tv_sec-start.tv_sec), (stop.tv_usec-start.tv_usec)/(int)(1000));
	///

	return get_server_reply();
}
int ftp_cwd(char* dir)
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "CWD %s\n", dir);

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
int ftp_test()
{
	if(cc_status())
	{	
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "ABOR\n");
		printf("%s", message);


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
int ftp_type()
{
	if(cc_status())
	{	
		if (server_send(get_cc_socket(), "TYPE I N\n", 9)<0)
		{
			printf("Error sending the TYPE command\n");
			return 0;
		}
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
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "MKD %s\n", dir);

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
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "RMD %s\n", dir);

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
		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "DELE %s\n", file);

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

		char *message = (char*)malloc(sizeof(char)*50);
		sprintf(message, "STOR %s\n", file);

		if (server_send(get_cc_socket(), message, strlen(message))<0)
		{
			printf("Error sending the STOR command\n");
			free(message);
			return 0;
		}
		free(message);

		if(get_server_reply())
		{
			return 0;
		}

		///	

		unsigned int dc_socket;
		unsigned int size;
		unsigned char* data;

		size_t bytes;

		struct timeval stop, start;

		dc_socket = get_dc_socket();
		size = sizeof(unsigned char);
		data = (unsigned char*)malloc(sizeof(unsigned char));

		bytes = 0;
		gettimeofday(&start, 0);
		//send data byte by byte till the end of the file
		do
		{
			if(fread(data, size, 1, ptr)-1)
			{
				gettimeofday(&stop, 0);
				break;
			}
			data_send(dc_socket, data, size);
			bytes++;
		}while(1);

		//disconnect from data connection toi
		//indicate completned of sending data
		server_disconnect(dc_socket);

		fclose(ptr);
		free(data);

		get_server_reply();

		//print sending raport
		printf("%lu bytes sent in ", bytes);
		ret_time((stop.tv_sec-start.tv_sec), (stop.tv_usec-start.tv_usec)/(int)(1000)); 
		ret_speed(bytes, (stop.tv_sec-start.tv_sec), (stop.tv_usec-start.tv_usec)/(int)(1000));
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
		if (server_send(get_cc_socket(), "QUIT\n", strlen("QUIT\n"))<0)
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
		if (server_send(get_cc_socket(), "HELP\n", strlen("HELP\n"))<0)
		{
			printf("Error sending the HELP command\n");
			return 0;
		}

		char* server_reply = malloc(sizeof(char)*501);
		control_receive(server_reply);
		printf("%s",server_reply);

		int i=0;
		while(strcmp((server_reply+(i)), "\n"))
		{
			strcpy((server_reply+(i++)), "");
		}
		strcpy((server_reply+(i++)), "");
		free(server_reply);

		return 0;
	}
	printf("open [IP] [PORT]\n*For IP - supports only IPv4\n*For PORT (optional field) for \n\tdefault port 21, write for other\nQuit => close the client\n");
	return 0;
}

//Parse data port from EPSV reply
int fetch_data_port(char* sr)
{
	char** args = split_to_array(sr, "(", 2);
	char** bars_code = split_to_array(*(args+1), "|", 2);

	int data_code = conv_to_num(*(bars_code));
	destroy(bars_code, 2);
	destroy(args, 2);

	if(data_code)
	{
		return data_code;
	}
	return 0;
}

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
//            ALLO <SP> <decimal-integer>
//                [<SP> R <SP> <decimal-integer>] <CRLF>
//            REST <SP> <marker> <CRLF>
//            RNFR <SP> <pathname> <CRLF>
//            RNTO <SP> <pathname> <CRLF>
//            ABOR <CRLF>
//            NLST [<SP> <pathname>] <CRLF>
//            SITE <SP> <string> <CRLF>
//            STAT [<SP> <pathname>] <CRLF>
//            noop <crlf>
