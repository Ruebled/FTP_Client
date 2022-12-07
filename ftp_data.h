#ifndef FTP_DATA_H
#define FTP_DATA_H

struct server_status
{
	int cc_status;//control connection status(is_connected)
	int dc_status;//data connection status(is_connected)
	int control_connection_socket;
	int data_connection_socket;
	char* session_ip;
};

void create_server_status();
void destroy_server_status();
int cc_status();

void cc_connected();

void cc_disconnected();

int dc_status();

void dc_connected();

void dc_disconnected();
void set_cc_socket(int sock_addr);
void set_dc_socket(int sock_addr);
int get_cc_socket();
int get_dc_socket();
char* get_session_ip();
void set_session_ip(char* IP);

#endif
