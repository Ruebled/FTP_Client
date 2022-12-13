#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#include "ftp_data.h"

//AF_INET represent address family, mean uses IPv4
//SOCK_STREAM means connection oriented protocol aka. TCP
//try on creating a socket
//


//define structure for ftp connection needed data
struct sockaddr_in server;


//create socket address for "control" connection 
int create_cc_socket()
{
	int cc_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (cc_socket_desc>0)
	{
		set_cc_socket(cc_socket_desc);
		return cc_socket_desc;
	}
	return -1;
}

//create socket address for "data" connection
int create_dc_socket()
{
	int dc_socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (dc_socket_desc>0)
	{
		set_dc_socket(dc_socket_desc);
		return dc_socket_desc;
	}
	return -1;
}

//Connect to either control or data port
int server_connect(int socket_desc, char *IP, int PORT)
{
	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET; server.sin_port = htons(PORT);
	
	return connect(socket_desc, (struct sockaddr *)&server, sizeof(server));
}
int server_disconnect(int socket_desc)
{
	if(close(socket_desc)>-1)
	{
		dc_disconnected();
	}
	return 0;
}

//send message to a given socket addr (control/data connection)
int server_send(int socket_desc, char *message, int message_len)
{
	return send(socket_desc, message, message_len, 0);
}

int data_send(int socket_desc, unsigned char *message, int message_len)
{
	return send(socket_desc, message, message_len, 0);
}

//get message from server via control connection
char *control_receive()
{
	char* server_reply = (char*)malloc(sizeof(char)*1000);

	recv(get_cc_socket(), server_reply, 1000, 0);

	return server_reply;
}

//get message from server via data connection
char *data_receive(int len)
{
	char *server_data = (char*)malloc(1);
	if((recv(get_dc_socket(), server_data, 1, 0)<1))
	{
		dc_disconnected();
	}

	return server_data;
}
