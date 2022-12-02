#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ftpcommands.h"
#define comd_count 4
#define max_param 3

char *commands[] = 
{
	"OPEN",
	"SEND",
	"RECV",
	"QUIT"
};

char* toup(char *str)
{
	char *temp = malloc(strlen(str));

	while(str)
	{
		*temp = toupper(*str);
		temp++;
	}
	strlcpy(temp, str, (strlen(str)+1));
	free(temp);
	return str;
}


int try_execute(char * str)
{
	char *den_com;

	const char *ch = " ";

	den_com = toup(strtok(str, ch));

	int i=-1;
	
	for (int i=0; i < comd_count; i++)
	{
		if (strcmp(den_com, commands[i]))
		{
			break;
		}
	}
	//change to 2 after
	char *param[max_param];
	int j = 1;
	while( den_com != NULL || j > max_param)
	{
		den_com = strtok(NULL, ch);
		j++;
	}

	int response = 0;

	switch (i)
	{
		case 0:
			ftp_open(param[max_param-1]);
			break;
		case 1:
			ftp_send(param[max_param-1]);
			break;
		case 2:
			break;
		default:
			response = 0;
			break;
	}

	return response;
}
