#ifndef FTPCOMMANDS_H
#define FTPCOMMANDS_H

int establish_control_connection(char* IP, int PORT);
int establish_data_connection();
int handle_response(char* resp_message);

int ftp_open(char** args);
int ftp_user();
int ftp_passwd();
int ftp_ls(char*);
int ftp_cwd(char*);
int ftp_cdup();
int ftp_pwd();
int ftp_mkd(char*dir);
int ftp_rmd(char*dir);
int ftp_dele(char*file);
int ftp_syst();
int ftp_type();

int ftp_retr(char*file);
int ftp_stor(char*file);

int ftp_quit();


int fetch_data_port(char* sr);
int ftp_help();

int get_server_reply();

int ftp_test();
#endif
