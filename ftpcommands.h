#ifndef FTPCOMMANDS_H
#define FTPCOMMANDS_H

int establish_control_connection(char* IP, int PORT);
int establish_data_connection();
int handle_response(char* resp_message);

int ftp_open(char** args);
int ftp_user();
int ftp_passwd();
int ftp_ls();
int ftp_cwd();
int ftp_cdup();
int ftp_pwd();
int ftp_mkd();
int ftp_rmd();
int ftp_dele();
int ftp_syst();

int ftp_retr();
int ftp_stor();

int ftp_quit();


int fetch_data_port(char* sr);
int ftp_help();

#endif
