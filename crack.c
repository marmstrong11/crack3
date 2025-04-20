#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "md5.h"

#if __has_include("fileutil.h")
#include "fileutil.h"
#endif

#define PASS_LEN 50     // Maximum length any password will be.
#define HASH_LEN 33     // Length of hash plus one for null.

int alphanumeric(const void *a, const void *b);
int findIndex(char *key, char **arr, int size);


int main(int argc, char *argv[])
{
    if(argc < 3) 
    {
        printf("Usage: %s hash_file dictionary_file\n", argv[0]);
        exit(1);
    }

    int size;
    char **hashes = loadFileAA(argv[1], &size);

    qsort(hashes, size, sizeof(char*), alphanumeric);

    int *alreadyCracked = malloc(size * sizeof(int)); //implemented to prevent double-counting of duplicates
    memset(alreadyCracked, 0, size * sizeof(int)); //sets all values to 0
    
    FILE *dict = fopen(argv[2], "r");
    if(!dict)
    {
        printf("File could not open.\n");
        exit(1);
    }
    
    char password[PASS_LEN];
    int cracked = 0;

    while (fgets(password, PASS_LEN, dict))
    {
        char *nl = strchr(password, '\n');
        if(nl) *nl = '\0';

        char *md5Hash = md5(password, strlen(password));
        char *found = bsearch(md5Hash, hashes, size, sizeof(char*), strsearch);

        if(found)
        {
            int index = findIndex(md5Hash, hashes, size);
            if(index != -1 && !alreadyCracked[index])
            {
                printf("%s  %s\n", password, *(char**)found);
                alreadyCracked[index] = 1;
                cracked++;
            }
        }
        else
        {
            int done = 0;
            char passCopy[PASS_LEN];
            strcpy(passCopy, password);
            char *md5Copy = NULL;

            for (int i = 0; i < 100 && !done; i++) //appends numbers 1-99 to the password
            {
                sprintf(passCopy, "%s%d", password, i);
                md5Copy = md5(passCopy, strlen(passCopy));
                found = bsearch(md5Copy, hashes, size, sizeof(char*), strsearch);

                if(found)
                {
                    int index = findIndex(md5Copy, hashes, size);
                    if(index != -1 && !alreadyCracked[index])
                    {
                        printf("%s  %s\n", passCopy, *(char**)found);
                        alreadyCracked[index] = 1;
                        cracked++;
                        done = 1;
                    }
                }
                free(md5Copy);

                if(done)
                {
                    break;
                }

                sprintf(passCopy, "%s%d!", password, i); //appends "!" after the numbers
                md5Copy = md5(passCopy, strlen(passCopy));
                found = bsearch(md5Copy, hashes, size, sizeof(char*), strsearch);

                if(found)
                {
                    int index = findIndex(md5Copy, hashes, size);
                    if(index != -1 && !alreadyCracked[index])
                    {
                        printf("%s  %s\n", passCopy, *(char**)found);
                        alreadyCracked[index] = 1;
                        cracked++;
                        done = 1;
                    }
                }
                free(md5Copy);
            }

            if(!done)
            {
                strcpy(passCopy, password);
                passCopy[0] = toupper(passCopy[0]); //capitalizes first letter
                md5Copy = md5(passCopy, strlen(passCopy));
                found = bsearch(md5Copy, hashes, size, sizeof(char*), strsearch);

                if(found)
                {
                    int index = findIndex(md5Copy, hashes, size);
                    if(index != -1 && !alreadyCracked[index])
                    {
                        printf("%s  %s\n", passCopy, *(char**)found);
                        alreadyCracked[index] = 1;
                        cracked++;
                    }
                }
                else
                {
                    strcpy(passCopy, password);
                    for (int i = 0; passCopy[i] != '\0'; i++)
                    {
                        passCopy[i] = toupper(passCopy[i]); //capitalizes all characters
                    }
                    
                    free(md5Copy);

                    md5Copy = md5(passCopy, strlen(passCopy));
                    found = bsearch(md5Copy, hashes, size, sizeof(char*), strsearch);

                    if(found)
                    {
                        int index = findIndex(md5Copy, hashes, size);
                        if(index != -1 && !alreadyCracked[index])
                        {
                            printf("%s  %s\n", passCopy, *(char**)found);
                            alreadyCracked[index] = 1;
                            cracked++;
                        }
                    }
                }
                free(md5Copy);
            }
        }

        free(md5Hash);
    }

    fclose(dict);
    printf("Hashes found: %d\n", cracked);
    free(alreadyCracked);
    freeAA(hashes, size);
}

char ** loadFileAA(char *filename, int *size)
{
	FILE *in = fopen(filename, "r");
	if(!in)
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

int strsearch(const void *key, const void *elem)
{
	return strcmp((const char*)key, *(const char**)elem); //elem points to a char pointer
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

int alphanumeric(const void *a, const void *b)
{
    char **aa = (char**)a;
    char **bb = (char**)b;

    return strcmp(*aa, *bb);
}

int findIndex(char *key, char **arr, int size)
{
    char **found = bsearch(key, arr, size, sizeof(char*), strsearch);

    if(!found)
    {
        return -1;
    }

    return found - arr; //index of hash
}

/*Results:

Hash list       enable1.txt     rockyou100k.txt     rockyou3m.txt (all slow down & time out, likely due to low RAM)
example0.txt         24               91                                up to "recon0645"
hashes1830.txt       33               68                                up to "Davide90"    
hashes2130.txt        2               16                                up to "lesmar22"
hashes3000.txt      489             3000                                up to "kadena"
hashes5000.txt      300              918                                up to "a6d34n0r"
*/