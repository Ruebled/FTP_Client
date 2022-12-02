#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H

int create_socket();

int server_connect(char*, int);

int server_send(char *);

char* server_receive();

#endif
