#ifndef ADIT_F_H
#define ADIT_F_H

int establish_control_connection(char* IP, int PORT);

int establish_data_connection();

int handle_response(char* resp_message);

void ret_time(int stop, int start);

void ret_speed(int bytes, int sec, int msec);

void toUP(char*);

int is_occupied(char* [], int count);

int conv_to_num(char*);

char** split_to_array(char*, const char*, int);

void destroy(char** args, int count);

int fetch_data_port(char* sr);

char* get_server_reply();

#endif
