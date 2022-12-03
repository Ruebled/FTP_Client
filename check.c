#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "check.h"

//remember keeping QUIT as the last word in the array!!!
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

#define arrow 4
int command_param_check[][arrow] =
{
//  Defined like:
//  P, M, F, S ->
	{3, 2, 1, 2},
	{2, 1, 0, 0},
	{2, 1, 0, 0},
	{2, 1, 0, 0},
	{2, 1, 0, 0}
// P -> posible number of arguments
// M -> mandatory number of present arguments
// F -> check function to be performed on first argument
// S -> check function to be performed on second argument
};
	
//check for input string to be an well written
//command for send to server
int check_input_validity(char *inp)
{
	//create array for storing each work of the char* inp
	const char *ch = " ";
	char *token;
	char* args[4];
	for (int i=0; i<4; i++)
	{
		args[i] = malloc(100*sizeof(char));
	}
	
	int count = 0;

	token = strtok(inp, ch);

	while( token != NULL && count<4)
	{
		args[count++] = token;
		token = strtok(NULL, ch);	
	}
	//

	//check if the command exist in the defined ones
	//par_ev take
	//			-1 if command is not defined
	//			a natural number representing command index position in
	//				available_commands array
	int par_ev = check_command(args[0]);	
	//

	//if block to intrerupt current function in case of
	// -1 -> not existing command
	// last index of the "available_commands" here defined as QUIT
	if (par_ev == -1)
	{
		printf("?Undefined command\n");
		return 0;
	}	
	else if (par_ev == ((sizeof(available_commands)/8)-1)) return -1;

	//check for the number of the given arguments
	//checked with a list of P.M.F.S(see at the top)
	//arguments for each existing command
	int is_full;
	is_full = is_occupied(args);
	if (is_full>command_param_check[par_ev][0]) 
	{
		printf("Too many arguments\n");
		return 0;
	}
	else if (is_full<command_param_check[par_ev][1])
	{
		printf("Not enough arguments\n");
		return 0;
	}
	else
	{
		printf("Good rigth enough parameters\n");
	}
	//

	if(command_param_check[par_ev][1] == 1 && !check_ip(args[1])) 
	{
		printf("Bad IP\n");
		return 0;
	}

	if(command_param_check[par_ev][2] == 2 && !check_port(args[2])) 
	{
		printf("Bad PORT\n");
		return 0;
	}

	if(command_param_check[par_ev][1] == 3 && !check_local_filename(args[1])) 
	{	
		printf("Filename not exists in current directory\n");
		return 0;
	}

	return 1;
}

//function that check the equallity between the given word as command
//and the existing command in the available_commands array
int check_command(char *com)
{
	toUP(com);
	for (int i=0; i<sizeof(available_commands)/8; i++)
	{
		if (!strcmp(com, available_commands[i])) 
		{
			return i;
		}
	}
	return -1;
}
//

//check for rightness of the ip here for IPv4
//four integer numbers delimited by a dot
//each number ranges between [0, 255]
//param_check index 1
int check_ip(char* ip)
{
	const char *ch = ".";
	char *token;
	char* args[4];
	for (int i=0; i<4; i++)
	{
		args[i] = malloc(50*sizeof(char));
	}
	
	int count = 0;

	token = strtok(ip, ch);

	while( token != NULL)
	{
		if (count>3) return 0;
		args[count++] = token;
		token = strtok(NULL, ch);	
	}

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
	return 0;	
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

int is_occupied(char *args[])
{
	for (int i=0; i<4; i++)
	{
		if (*args[i] == '\0') return i;
	}
	return 4;
}

int conv_to_num(char* str)
{
	//return the converted number
	//-1 otherwise
	return -1;
}
