#ifndef SOCKETFUNC_H
#define SOCKETFUNC_H

int create_cc_socket();

int create_dc_socket();

int server_connect(int socket_desc , char* ip, int port);

int server_disconnect(int socket_desc);

int control_send(int socket_desc, char* message_to_send, int message_lenght);

void control_receive(int socket_desc, char* buffer, int buff_size);

int buff_send(int socket_desc, unsigned char* message_to_send, int message_lenght);

void buff_receive(int socket_desc, unsigned char* buffer, int buff_size);

#endif
