#include <ctype.h>
#include <string.h>

//Fuctions for triming of spaces the input command
char *ltrim(char *s)
{
	while(isspace(*s)) s++;
	return s;
}

char *rtrim(char *s)
{
	char * back = s + strlen(s);
	while(isspace(*--back) || !strcmp(back, "\n") || !strcmp(back, "."));
	*(back+1) = '\0';
	return s;
}

char *trim(char *s)
{
	return rtrim(ltrim(s));
}
