#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H

int create_socket();

int server_connect(int, char*, int);

int server_send(int, char*, int);

char* server_receive();

#endif
