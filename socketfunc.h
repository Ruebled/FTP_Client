#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H
#include <stdio.h>

int create_cc_socket();

int create_dc_socket();

int server_connect(int, char*, int);

int server_send(int, char*, int);

char* control_receive();

char* data_receive();

#endif
