#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>

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

//variable for returning the function execution response
int response;

//Connect to either control or data port
int server_connect(int socket_desc, char *IP, int PORT)
{
	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET; server.sin_port = htons(PORT);
	
	response = connect(socket_desc, (struct sockaddr *)&server, sizeof(server));

	return response;
}

//send message to a given socket addr (control/data connection)
int server_send(int socket_desc, char *message, int message_len)
{
	response = send(socket_desc, message, message_len, 0);
	return response;
}

#define message_len 2000
char* server_reply;
//get message from server via control connection
char *control_receive()
{
	server_reply = (char*)malloc(sizeof(char)*message_len);

	recv(get_cc_socket(), server_reply, message_len, 0);

	return server_reply;
}

#define data_len 2000
//get message from server via data connection
char *data_receive()
{
	server_reply = (char*)malloc(sizeof(char)*data_len);

	recv(get_dc_socket(), server_reply, data_len, 0);

	return server_reply;
}
