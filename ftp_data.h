#ifndef FTP_DATA_H
#define FTP_DATA_H

struct server_status
{
	int is_connected;
	int socketaddr;


};

struct server_status* create_server_status();

#endif
