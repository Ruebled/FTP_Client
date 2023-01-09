#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#include <fcntl.h>
#include <sys/time.h>

#include "include/ftp_data.h"

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
	int res; 
	long arg; 
	fd_set myset; 
	struct timeval tv; 

	int valopt; 
	socklen_t lon; 

	server.sin_addr.s_addr = inet_addr(IP);
	server.sin_family = AF_INET; 
	server.sin_port = htons(PORT);

	// Set non-blocking 
	if((arg = fcntl(socket_desc, F_GETFL, NULL)) < 0) 
	{ 
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
		exit(0);
	}	
	arg |= O_NONBLOCK; 
	if (fcntl(socket_desc, F_SETFL, arg) < 0) 
	{ 
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
		exit(0); 
	} 
	// Trying to connect with timeout 
	res = connect(socket_desc, (struct sockaddr *)&server, sizeof(server)); 

	if (res < 0)
	{ 
		if (errno == EINPROGRESS) 
		{ 
			fprintf(stderr, "Connecting in progress\n"); 
			do 
			{ 
				tv.tv_sec = 8; 
				tv.tv_usec = 0; 
				FD_ZERO(&myset); 
				FD_SET(socket_desc, &myset); 
				res = select(socket_desc+1, NULL, &myset, NULL, &tv); 

				if (res < 0 && errno != EINTR) 
				{ 
					fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
					res = -1;
					break; 
				} 
				else if (res > 0)
				{ 
					// Socket selected for write 
					lon = sizeof(int); 
					if (getsockopt(socket_desc, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) 
					{ 
						fprintf(stderr, "Error in getsockopt %d - %s\n", errno, strerror(errno)); 
						res = -1;
						break; 
					} 
					// Check the value returned... 
					if (valopt) 
					{ 
						fprintf(stderr, "Error in delayed connection %d - %s\n", valopt, strerror(valopt)); 
						res = -1;
						break;
					} 
					break; 
				} 
				else
				{ 
					fprintf(stderr, "Timeout reached\n");
					res = -1;
					break;
				} 
			} while (1); 
		} 
		else
		{
			fprintf(stderr, "Error connecting %d - %s\n", errno, strerror(errno)); 
			res = -1;
		}

	} 
	// Set to blocking mode again... 
	if ((arg = fcntl(socket_desc, F_GETFL, NULL)) < 0) 
	{ 
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno)); 
		exit(0); 
	} 
	arg &= (~O_NONBLOCK); 
	if( fcntl(socket_desc, F_SETFL, arg) < 0) 
	{ 
		fprintf(stderr, "Error fcntl(..., F_SETFL) (%s)\n", strerror(errno)); 
		exit(0); 
	}
	return res;
}

int server_disconnect(int socket_desc)
{
	if(close(socket_desc)>-1)
	{
		dc_disconnected();
	}
	return 0;
}

//get message from server via control connection
int control_receive(int socket_desc, char* server_reply, int buff_s)
{
	return recv(socket_desc, server_reply, buff_s, 0);
}

void control_send(int socket_desc, unsigned char * message, int buff_s)
{
	 if (send(socket_desc, message, buff_s, 0)<0)
	 {
		 cc_disconnected();
	 }
}

//send message to a given socket addr (control/data connection)
int buff_send(int socket_desc, unsigned char *message, int message_len)
{
	return send(socket_desc, message, message_len, 0);
}

int buff_receive(int socket_desc, unsigned char *server_data, int buff_s)
{
	int size = recv(socket_desc, server_data, buff_s, 0);
	if(size < 1)
	{
		dc_disconnected();
	}
	return size;
}

