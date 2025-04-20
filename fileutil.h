// Load the dictionary of words from the given filename.
// Return a pointer to the array of strings.
// Sets the value of size to be the number of valid
// entries in the array (not the total array length).
char ** loadFileAA(char *filename, int *size);


// Search the dictionary for the target string
// Return the found string or NULL if not found.
char * substringSearchAA(char *target, char ** arr, int size);


// Free the memory used by the array
void freeAA(char ** arr, int size);

int strsearch(const void *key, const void *elem);