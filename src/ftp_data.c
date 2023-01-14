#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/ftp_data.h"
#include "include/ftpcommands.h"
#include "include/socketfunc.h"
#include "include/misc_func.h"


struct server_status status;

void create_server_status()
{
	status.cc_status = 0;
	status.dc_status = 0;
	status.control_connection_socket = -1;
	status.data_connection_socket = -1;
}

void destroy_server_status()
{
	if(get_session_ip())
	{
		free(status.session_ip);
	}
}

//set session ip
void set_session_ip(char* IP)
{
	status.session_ip = (char*)malloc(strlen(IP)+1);
	memcpy(status.session_ip, IP, strlen(IP)+1);
}

char* get_session_ip()
{
	return status.session_ip;
}

//return control connection status
int cc_status()
{
	return status.cc_status;
}

//change control connection to connected
void cc_connected()
{
	status.cc_status = 1;
}

//change control connection to disconnected
void cc_disconnected()
{
	status.cc_status = 0;
}

//get data connection status, usually disconnected...
int dc_status()
{
	return status.dc_status;
}

//set data connection status to connected
void dc_connected()
{
	status.dc_status = 1;	
}

//set data connection status to disconnected
void dc_disconnected()
{
	status.dc_status = 0;
}


//set control connection socket
void set_cc_socket(int sock_addr)
{
	status.control_connection_socket = sock_addr;
}

//set data connection socket
void set_dc_socket(int sock_addr)
{
	status.data_connection_socket = sock_addr;
}

//get control connection socket address
int get_cc_socket()
{
	return status.control_connection_socket;
}

//get data connection socket address
int get_dc_socket()
{
	return status.data_connection_socket;
}
