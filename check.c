#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "check.h"

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
//  Defined like:
//  P, M, F, S ->
	{2, 1, 2},
	{1, 0, 0},
	{1, 0, 0},
	{1, 0, 0},
	{1, 0, 0}
// P -> posible number of arguments
// M -> mandatory number of present arguments
// F -> check function to be performed on first argument
// S -> check function to be performed on second argument
};
	
//if command == QUIT then exit(0)
int check_input_validity(char* inp)
{
	const char *ch = " ";
	char *token;
	char* args[4];
	for (int i=0; i<4; i++)
	{
		args[i] = malloc(100*sizeof(char));
	}
	
	int count = 0;

	token = strtok(inp, ch);
	args[count] = token;

	while( token != NULL || ++count<4)
	{
		token = strtok(NULL, ch);	
		args[count] = token;
	}

	int par_ev = check_command(args[0]);	
	
	if (par_ev == -1)
	{
		printf("?Undefined command\n");
		return 0;
	}
	//if the last command of available_commands list
	//aka QUIT is found, then return -1, that
	//will break the main while loop
	else if (par_ev == sizeof(available_commands)) return -1;
	
	int is_full;
	is_full = is_occupied(args[4]);
	if (is_full>command_param_check[par_ev][0] && is_full<command_param_check[1])
	{
		printf("Too many arguments");
		return 0;
	}
	else if (is_full<command_param_check[par_ev][1])
	{
		printf("Not enough arguments");
	}
	else
	{
		printf("Good rigth command");
		
		//call socket send
		//or return something meaningfull
		//like the args array
	}
}

int check_command(char *com)
{
	char *com_UPCASE;
	com_UPCASE = toUP(com);
	for (int i=0; i<sizeof(available_commands); i++)
	{
		if (!strcmp(com_UPCASE, available_commands[i])) return i;
	}
	return -1;
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

//some tools function maybe move in 
//different files latter
char* toUP(char* comd)
{
	char *up_temp = malloc(sizeof(char)*strlen(comd));

	for (int i=0; i<strlen(comd); i++)
	{
		*(up_temp+i) = toupper(comd[i]);
	}
	strcpy(comd, up_temp);
	free(up_temp);
	return comd;
}
