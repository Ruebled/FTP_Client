#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>

#include "ftp_data.h"

#define message_length 2000

//AF_INET represent address family, mean uses IPv4
//SOCK_STREAM means connection oriented protocol aka. TCP
//try on creating a socket


//define structure for ftp connection needed data
struct sockaddr_in server;

//variable for returning the function execution response
static int response;

int create_socket()
{
	int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_desc>0)
	{
		change_socket_addr(socket_desc);
		return socket_desc;
	}
	else
	{
		return -1;
	}
}

int server_connect(int socket_desc, char *IP, int PORT)
{
	//basic definitions of the server 
	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	
	response = connect(socket_desc, (struct sockaddr *)&server, sizeof(server));
	
	//printf("%d\n", errno);

	return response;
}


int server_send(int socket_desc, char *message, int message_len)
{
	response = send(socket_desc, message, message_len, 0);
	return response;
}

char server_reply[2000];

char *server_receive()
{
	int socket_desc = get_socket_addr();
	recv(socket_desc, server_reply, message_length, 0);
	return server_reply;
}
