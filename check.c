#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* available_commands[] =
{	
	"OPEN",
	"LS",
	"PASS",
	"CWD",
	"RMD",
	"MKD",
	"PWD",
	"RETR",
	"STOR",
	"LIST",
	"ABOR",
	"QUIT"
};

int command_param_check[][3] =
{
	{2, 1, 2},
	{1, 0, 0},
	{1, 0, 0},
	{1, 0, 0},
	{1, 0, 0}
};
	
//if command == QUIT then exit(0)
int check_input_validity(char* inp)
{
	char ch = " ";
	char* args[5];
	for (int i=0; i<5; i++)
	{
		args[i] = malloc(100*sizeof(char));
	}
	
	int count = 0;
	strcpy(args[0], strtok(inp, ch));
	


	
}

int check_ip(char* ip)
{
}

int check_port(char* port)
{
	
}

int check_local_filename(char* filename)
{
}
