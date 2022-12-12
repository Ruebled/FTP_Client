#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H
#include <stdio.h>

int create_cc_socket();

int create_dc_socket();

int server_connect(int, char*, int);

int server_disconnect(int);

int server_send(int, char*, int);
int data_send(int, unsigned char*, int);

char* control_receive();

unsigned char* data_receive();

#endif
