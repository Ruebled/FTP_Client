#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/time.h>
#include <unistd.h>

#include "include/trim.h"
#include "include/socketfunc.h"
#include "include/misc_func.h"
#include "include/ftp_data.h"
#include "include/ftpcommands.h"

#define buff_size 1024
#define buff_reply 128


char* get_server_reply()
{
	char* server_reply = (char*)malloc(sizeof(char)*buff_reply);
	memset(server_reply, 0x00, buff_reply);
	control_receive(get_cc_socket(), server_reply, buff_reply);
	if(!strcmp(server_reply, ""))
	{
		server_disconnect(get_cc_socket());
		cc_disconnected();
		set_dc_socket(-1);
		set_cc_socket(-1);
		set_session_ip("");

		return server_reply;
	}

	return server_reply;
}

//Try to connect to a server in control connection
int establish_control_connection(char* IP, int PORT)
{
	if(create_cc_socket()<0)
	{
		printf("Couldn't create socket\n");
		return -1;
	}			
	if(server_connect(get_cc_socket(), IP, PORT)<0)
	{
		printf("Couln't connect to the server\n");
		return -1;
	}
	
	char* server_resp = get_server_reply();
	int res = handle_response(server_resp);
	free(server_resp);
	return res;
}

//Try to connect via data channel
int establish_data_connection()
{
	if(!create_dc_socket())
	{
		printf("Can't create data connection socket\nStrange....\n");
		return 0;
	}

	//Change to work with define comd_term above
	if (control_send(get_cc_socket(), "EPSV\r\n", strlen("EPSV\r\n"))<0)
	{
		printf("Error sending the EPSV command\n");
		return 0;
	}
	
	char* server_reply = get_server_reply(); 
	int res = handle_response(server_reply); 
	int data_port = 0;
	if (res == 2)
	{
		data_port = fetch_data_port(server_reply);
	}

	free(server_reply);

	if (!data_port)
	{
		return 0;
	}

	if(server_connect(get_dc_socket(), get_session_ip(), data_port)<0)
	{
		printf("Couldn't connect to data server\n");
		return 0;
	}

	dc_connected();

	return 1;
}

//Do response action to any reply server code
int handle_response(char* sr)
{
	if(!strcmp(sr, ""))
	{
		return 0;
	}
	char** reply = split_to_array(sr, " ", 1);

	int reply_code = conv_to_num(*reply);

	destroy(reply, 1);

	switch(reply_code)
	{
		case 257:
		case 250:
		case 215:
		case 502:
		case 221:
		case 150:
		case 214:
			return 0;

		case 200:
			return 1;
		case 226:
		case 450:
		case 550:
			dc_disconnected();
			return 0;
		case 220:
			return ftp_user();

		case 229:
			return 2;

		case 331:
			return ftp_passwd();

		case 230:
			cc_connected();
			printf("Connected to the server\n");
			return 0;

		case 332:
			printf("ACCT info required\nNot yet created the function\n");
			return 0;
		case 553:
			dc_disconnected();
			return -1;
		case 421:
		case 530:
			cc_disconnected();
			return -1;

		default:
			printf("Unknown return code %d\n", reply_code);
			return 0;
	} 
}

void ret_time(int sec, int usec)
{
	if(usec<0) usec=-usec;

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
	if (usec)
	{
		int msec = (int)(usec/1000);
		if(msec)
		{
			printf("%d %s ", msec, ((msec>1)?"miliseconds":"milisecond"));
			return;
		}
		printf("%d %s ", usec, ((usec>1)?"microseconds":"microsecond"));
	}
}

void ret_speed(int bytes, int sec, int usec)
{
	long int speed = (int)((double)bytes/((double)sec+(double)usec/(double)1000000));

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
		printf("(%ld byte/sec)\n", speed);
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
	int i;
	for (i=0; i<count; i++)
	{
		if (!strcmp(*(args+i),"")) return i;
	}
	return i;
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

char**  split_to_array(char* inputstr, const char *ch, int count)
{
	int input_len = strlen(inputstr);
	char* str = malloc(sizeof(char)*(input_len+1));
	strcpy(str, inputstr);
	
	char** args = (char**)malloc(sizeof(char*)*count);
	for(size_t i=0; i<count; i++)
	{
		*(args+i) = (char*)malloc(sizeof(char)*buff_size);
		memset(*(args+i), 0x00, buff_size);
	}

	char* token = strtok(str, ch);
	
	size_t inc = 0;
	while(token != NULL && inc<count)
	{
		trim(token);

		strcpy(*(args+inc), token);
		//*(*(args+inc)+strlen(token)) = '\0';

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

	return args;
}


void destroy(char** reply, int count)
{
	for(int i=0; i<count; i++)
	{
		free(*(reply+i));
		*(reply+i) = NULL;
   	}
	free(reply);
}


//Parse data port from EPSV reply
int fetch_data_port(char* sr)
{
	char** args = split_to_array(sr, "(", 2);
	char** bars_code = split_to_array(*(args+1), "|", 2);

	int data_code = conv_to_num(*(bars_code));
	destroy(bars_code, 2);
	destroy(args, 2);

	if(data_code)
	{
		return data_code;
	}
	return 0;
}
