#ifndef FTPCOMMANDS_H
#define FTPCOMMANDS_H

int ftp_open(char** args);
int ftp_user();
int ftp_passwd();
int ftp_ls(char* path);
int ftp_cwd(char* path);
int ftp_cdup();
int ftp_pwd();
int ftp_mkd(char* dir);
int ftp_rmd(char* dir);
int ftp_dele(char* file);
int ftp_syst();
int ftp_type();
int ftp_retr(char* file);
int ftp_stor(char* file);
int ftp_quit();
int ftp_help();

int ftp_test();
#endif
