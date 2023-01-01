#include "include/misc_func.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include "include/trim.h"

void ret_time(int sec, int msec)
{
	if(msec<0) msec=-msec;

	if (sec/3600)
	{
		printf("%d %s ", sec/3600, ((sec/3600>1)?"hours":"hour"));
		sec %= 3600;
	}
	if (sec/60)
	{
		printf("%d %s ", sec/60, ((sec/60>1)?"minutes":"minute"));
		sec %= 60;
	}
	if (sec)
	{
		printf("%d %s ", sec, ((sec>1)?"seconds":"second"));
	}
	if (msec)
	{
		printf("%d %s ", msec, ((msec>1)?"miliseconds":"milisecond"));
	}
}

void ret_speed(int bytes, int sec, int msec)
{
	int speed = (int)((double)bytes/((double)sec+(double)msec/(double)1000));

	if (speed/1048576)
	{
		printf("(%lf mbyte/sec)\n", (double)speed/(double)1048576);
	}
	else if (speed/1024)
	{
		printf("(%lf kbyte/sec)\n", (double)speed/(double)1024);
	}
	else
	{
		printf("(%d byte/sec)\n", speed);
	}
}

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

char**  split_to_array(char* inputstr, const char *ch)
{
	int input_len = strlen(inputstr);
	char* str = malloc(sizeof(char)*(input_len+1));
	strcpy(str, inputstr);
	
	char** args = (char**)malloc(sizeof(char*)*4);
	for(size_t i=0; i<4; i++)
	{
		*(args+i) = (char*)malloc(sizeof(char)*100);
		strcpy(*(args+i), "");
	}

	int count=0;
	char* token = strtok(str, ch);
	
	while( token != NULL && count<4)
	{
		trim(token);

		strcpy(*(args+count++), token);

		token = strtok(NULL, ch);	
	}
	free(str);

	return args;
}

char**  split(char* inputstr)
{
	int input_len = strlen(inputstr);
	char* str = malloc(sizeof(char)*(input_len+1));
	strcpy(str, inputstr);
	
	char** args = (char**)malloc(sizeof(char*)*2);
	for(size_t i=0; i<2; i++)
	{
		*(args+i) = (char*)malloc(sizeof(char)*100);
		strcpy(*(args+i), "");
	}

	int count=0;
	char* token = strtok(str, " ");
	trim(token);

	while( token != NULL && count<2)
	{
		trim(token);

		strncpy(*(args+count++), token, strlen(token)+1);

		token = strtok(NULL, "");	
	}
	free(str);

	return args;
}

void destroy(char** reply)
{
	for(int i=0; i<4; i++)
	{
		free(*(reply+i++));
   	}
	free(reply);
}

void destroy2(char** reply)
{
	for(int i=0; i<2; i++)
	{
		free(*(reply+i++));
   	}
	free(reply);
}
