#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutil.h"


char ** loadFileAA(char *filename, int *size)
{
	FILE *in = fopen(filename, "r");
	if (!in)
	{
	    perror("Can't open file");
	    exit(1);
	}

	int capacity = 60;
	char ** arr = malloc(capacity * sizeof(char*));
	*size = 0;
	char line[1000];
	int index = 0;

	while(fgets(line, 1000, in))
	{
		char *nl = strchr(line, '\n');
		if(nl) *nl = '\0';

		if(*size == capacity)
		{
			capacity += 30;
			arr = realloc(arr, capacity * sizeof(char*));
		}

		char *str = malloc(strlen(line) + 1);

		strcpy(str, line); //str acts as the middleman between line and arr
		arr[index] = str;

		*size = *size + 1;
		index++;
	}

	fclose(in);

	return arr;
}

char * substringSearchAA(char *target, char **lines, int size)
{
	int index = 0;

	while(index < size)
	{
		if(strstr(lines[index], target))
		{
			return target; //target found
		}
		index++;
	}

	return NULL; //target not found
}

void freeAA(char ** arr, int size)
{
	int index = 0;

	while(index < size)
	{
		free(arr[index]);
		index++;
	}

	free(arr);
}

int strsearch(const void *key, const void *elem)
{
	return strcmp((char*)key, (char*)elem);
}