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
		printf("(%.3lf mbyte/sec)\n", (double)speed/(double)1048576);
	}
	else if (speed/1024)
	{
		printf("(%.3lf kbyte/sec)\n", (double)speed/(double)1024);
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

int is_occupied(char **args, int count)
{
	for (int i=0; i<count; i++)
	{
		if (!strcmp(*(args+i),"")) return i;
	}
	return count;
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
#define cell_size 100

char**  split_to_array(char* inputstr, const char *ch, int count)
{
	char* str = malloc(sizeof(char)*(strlen(inputstr)+1));
	strcpy(str, inputstr);
	
	char** args = (char**)malloc(sizeof(char*)*count);
	for(size_t i=0; i<count; i++)
	{
		*(args+i) = (char*)malloc(sizeof(char)*cell_size);
		memset(*(args+i), 0x00, cell_size);
	}

	char* token = strtok(str, ch);
	
	size_t inc = 0;
	while( token != NULL && inc<count)
	{
		trim(token);

		strcpy(*(args+inc), token);
		*(*(args+inc)+strlen(token)) = '\0';

		inc++;
		if(inc < count-1)
		{
			token = strtok(NULL, ch);	
		}
		else
		{
			token = strtok(NULL, "");	
		}
	}
	free(str);
	str = NULL;

	return args;
}

void destroy(char** reply, int count)
{
	for(int i=0; i<count; i++)
	{
		memset(*(reply+i), 0x00, cell_size);
		free(*(reply+i));
		*(reply+i) = NULL;
   	}
	free(reply);
	reply = NULL;
}
