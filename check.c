#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "check.h"
#include "ftpcommands.h"
#include "trim.h"



int check_command(char *com)
{
	//call the function name than is the same
	//as the first word in com
	
	char **args = split_to_array(com, " ");

	toUP(*args);

	if (!strcmp(*args, ""))
	{
		return 0;	
	}

	if (!strcmp(*args,"OPEN"))
	{
		if(ftp_open(args))
		{
			return 1;
		}
		return 0;
	}
	if (!strcmp(*args, "LS"))
	{
		if(ftp_ls(args))
		{
			return 1;
		}	
		return 0;
	}
	if (!strcmp(*args, "QUIT"))
	{
		return -1;
	}

	printf("Unknown command\n");
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
	char *temp= malloc(sizeof(char)*20);
	strcpy(temp, ip);
	const char *ch = ".";
	char *token;
	char* args[4];
	for (int i=0; i<4; i++)
	{
		args[i] = malloc(50*sizeof(char));
	}
	
	int count = 0;

	token = strtok(temp, ch);

	while( token != NULL)
	{
		if (count>3)
		{
			free(temp);
			return 0;
		}
		strcpy(args[count++], token);
		token = strtok(NULL, ch);	
	}
	free(temp);
	if (count<4) return 0; 

	//check if is number in range
	for (int i=0; i<4; i++)
	{
		int num = conv_to_num(args[i]);
		if (num<0 || num>255) return 0;
	}
	return 1;

	
	//free args array
	for (int i=0; i<4; i++)
	{
		free(args[i]);
	}
	return 0;
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
//

//Get the list of the files in local current working directory
//compare with the one given as the command parameter
//param_check index 3
int check_local_filename(char* filename)
{
	return 0;
}
//


//some tools function maybe move in 
//different files latter
void toUP(char* comd)
{
	for (int i=0; i<strlen(comd); i++)
	{
		*(comd+i) = toupper(*(comd+i));
	}
}

int is_occupied(char **args)
{
	for (int i=0; i<4; i++)
	{
		if (!strcmp(*(args+i),"")) return i;
	}
	return 4;
}

int conv_to_num(char* str)
{
	//return the converted number
	//if no more that 5 character lenght
	//-1 otherwise
	if (strlen(str)>5) return -1;

	for (int i=0; i<strlen(str); i++)
	{
		if (!isdigit(str[i]))
		{
			return -1;
		}	
	}
	return atoi(str);
}

char**  split_to_array(char* str, const char *ch)
{
	char *token;
	int count=0;
	char** args;
	args = (char**)malloc(sizeof(char*)*4);

	for (int i=0; i<4; i++)
	{
		*(args+i) = malloc(100*sizeof(char));
	}

	token = strtok(str, ch);
	
		while( token != NULL && count<4)
		{
			*(args+count++) = trim(token);
			token = strtok(NULL, ch);	
		}
	return args;
}
