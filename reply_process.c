#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ftpcommands.h"
#include "check.h"

#define y 4
#define x 1

struct command
{
	int *codes;


};

	//{"220","USER ","1","\n"},
	//{"331","PASS ","1","\n"}

//LIST [<SP> <pathname>] <CRLF>
//ACCT <SP> <account-information> <CRLF>
//CWD  <SP> <pathname> <CRLF>
//CDUP <CRLF>
//SMNT <SP> <pathname> <CRLF>
//QUIT <CRLF>
//REIN <CRLF>
//PORT <SP> <host-port> <CRLF>
//PASV <CRLF>
//TYPE <SP> <type-code> <CRLF>
//STRU <SP> <structure-code> <CRLF>
//MODE <SP> <mode-code> <CRLF>
//RETR <SP> <pathname> <CRLF>
//STOR <SP> <pathname> <CRLF>
//STOU <CRLF>
//APPE <SP> <pathname> <CRLF>
//ALLO <SP> <decimal-integer>
//[<SP> R <SP> <decimal-integer>] <CRLF>
//REST <SP> <marker> <CRLF>
//RNFR <SP> <pathname> <CRLF>
//RNTO <SP> <pathname> <CRLF>
//ABOR <CRLF>
//DELE <SP> <pathname> <CRLF>
//RMD  <SP> <pathname> <CRLF>
//MKD  <SP> <pathname> <CRLF>
//PWD  <CRLF>
//NLST [<SP> <pathname>] <CRLF>
//SITE <SP> <string> <CRLF>
//SYST <CRLF>
//STAT [<SP> <pathname>] <CRLF>
//HELP [<SP> <string>] <CRLF>
//NOOP <CRLF>
//};


int process_res(char* res)
{
	char* str = malloc(sizeof(char)*1000);
	while(1)
	{
		char *token = strtok(res, " ");	
		
	}

	free(str);
	return 0;
}
