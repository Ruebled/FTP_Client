#include <stdlib.h>

#include "ftp_data.h"


struct server_status *status;

void create_server_status()
{
	status = (struct server_status*)malloc(sizeof(struct server_status));

	status->is_connected = 0;
	status->socketaddr = -1;
}

int is_connected()
{
	return status->is_connected;
}

void toogle_is_connected()
{
	status->is_connected = !(status->is_connected);
}

int get_socket_addr()
{
	return status->socketaddr;
}

void change_socket_addr(int socket_addr)
{
	status->socketaddr = socket_addr;
}



struct server_status *data;

void create_server_data()
{
	data = (struct server_status*)malloc(sizeof(struct server_status));

	data->is_connected = 0;
	data->socketaddr = -1;
}

void change_socket_data_addr(int socket_addr)
{
	data->socketaddr = socket_addr;
}

int get_data_addr()
{
	return data->socketaddr;
}
