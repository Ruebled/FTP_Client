#include <stdio.h>
#include <string.h> 

#include "include/check.h"
#include "include/ftpcommands.h"
#include "include/trim.h"
#include "include/misc_func.h"

int check_command(char *com)
{ 
	char **args = split_to_array(com, " ", 2); 

	char **kargs = split_to_array(com, " ", 4);

	toUP(*args);

	if (!strcmp(*args, "TEST"))
	{
		ftp_test();
		return 0;
	}

	if (!strcmp(*args,"OPEN"))
	{
		if(ftp_open(kargs))
		{
			ftp_syst();
		}
	}
	else if (!strcmp(*args, "LS"))
	{
		ftp_ls(*(args+1));
	}
	else if (!strcmp(*args, "RETR"))
	{
		ftp_retr(*(args+1));
	}
	else if (!strcmp(*args, "STOR"))
	{
		ftp_stor(*(args+1));
	}
	else if (!strcmp(*args, "PWD"))
	{
		ftp_pwd();
	}
	else if (!strcmp(*args, "DELE"))
	{
		ftp_dele(*(args+1));
	}
	else if (!strcmp(*args, "RMD"))
	{
		ftp_rmd(*(args+1));
	}
	else if (!strcmp(*args, "CWD"))
	{
		ftp_cwd(*(args+1));
	}
	else if (!strcmp(*args, "CDUP"))
	{
		ftp_cdup();
	}
	else if (!strcmp(*args, "MKD"))
	{
		ftp_mkd(*(args+1));
	}
	else if (!strcmp(*args, "HELP"))
	{
		ftp_help();
	}
	else if (!strcmp(*args, "QUIT"))
	{
		if(ftp_quit()<0)
		{
			destroy(kargs, 4);
			destroy(args, 2);
			return -1;
		}
	}
	else if (!strcmp(*args, ""))
	{
		;;
	}
	else
	{
		printf("Unknown command\nTry HELP\n");
	}
	destroy(args, 2);
	destroy(kargs, 4);
	return 0;
}
//

//
//check for rightness of the ip here for IPv4
//four integer numbers delimited by a dot
//each number ranges between [0, 255]
//param_check index 1
int check_ip(char* ip)
{
	char** args;
	args = split_to_array(ip, ".", 5);

	int sets = is_occupied(args, 5);

	if (sets!=4)
	{
		destroy(args, 5);
		return 0; 
	}

	//check if number is in range
	for (int i=0; i<4; i++)
	{
		int num = conv_to_num(args[i]);
		if (num<0 || num>255) 
		{
			destroy(args, 5);
			return 0;
		}
	}
	destroy(args, 5);
	return 1;
}
//

//Check for port if written correctrly
//an integer number in the range of
//[0, 2^16-1]
//param_check index 2
int check_port(char* port)
{
	int res = conv_to_num(port);
	if (res<0 || res>65535) return 0;
	return res;	
}
