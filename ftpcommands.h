#ifndef FTPCOMMANDS_H
#define FTPCOMMANDS_H

int establish_control_connection(char* IP, int PORT);
int establish_data_connection();
int handle_response(char* resp_message);

int ftp_user();
int ftp_passwd();
int ftp_open(char** args);
int ftp_ls(char**);

int ftp_retr(char**);
int fetch_data_port(char* sr);

int ftp_quit();
#endif
