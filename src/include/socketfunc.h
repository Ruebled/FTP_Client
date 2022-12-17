#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H
#include <stdio.h>

int create_cc_socket();

int create_dc_socket();

int server_connect(int, char*, int);

int server_disconnect(int);

int server_send(int, char*, int);
int data_send(int, unsigned char*, int);

void control_receive(char*);

void data_receive(unsigned char*);

void info_receive(char*);

#endif
