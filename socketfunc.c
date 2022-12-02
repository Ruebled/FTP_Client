#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>

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
	return socket_desc;
}

int server_connect(int socket_desc, char *IP, int PORT)
{
	//basic definitions of the server localhost to 80 port if not provided
	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);
	
	response = connect(socket_desc, (struct sockaddr *)&server, sizeof(server));
	
	return response;
}


int server_send(int socket_desc, char *message, int message_len)
{
	response = send(socket_desc, message, message_len, 0);
	return response;
}

char server_reply[2000];

char *server_receive(int socket_desc)
{
	recv(socket_desc, server_reply, message_length, 0);
	return server_reply;
}
