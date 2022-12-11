#ifndef CHECK_H
#define CHECK_H

int check_ip(char*);

int check_port(char*);

int check_local_filename(char*);

int check_command(char*);

void toUP(char*);

int is_occupied(char* []);

int conv_to_num(char*);

char** split_to_array(char*, const char*);
void destroy(char**);
//make function to free the array 
//ftpcommands hadle response 35
//

//char* get_string(char**args);

#endif
