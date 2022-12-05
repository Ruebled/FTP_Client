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
