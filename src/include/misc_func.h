#ifndef ADIT_F_H
#define ADIT_F_H

void ret_time(int stop, int start);
void ret_speed(int bytes, int sec, int msec);
//void ret_name(char* file);


void toUP(char*);

int is_occupied(char* []);

int conv_to_num(char*);

char** split_to_array(char*, const char*);

char** split(char*);

void destroy(char** args);

void destroy2(char** args);
#endif
