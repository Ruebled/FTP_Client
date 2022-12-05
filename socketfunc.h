#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H

int create_socket();

int server_connect(int, char*, int);

int server_send(int, char*, int);

char* server_receive();

void create_server_data();

char* server_data_receive();

int create_data_socket();

#endif
