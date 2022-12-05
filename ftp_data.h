#ifndef FTP_DATA_H
#define FTP_DATA_H

struct server_status
{
	int is_connected;
	int socketaddr;


};

void create_server_status();

int is_connected();

void toogle_is_connected();

int get_socket_addr();

void change_socket_addr(int);

#endif
