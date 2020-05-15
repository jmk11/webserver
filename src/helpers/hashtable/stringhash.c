#include <stdlib.h>
#include <string.h>

#include "stringhash.h"

/*
* A hash function for hashing string keys
*/
unsigned int stringhash(const void *voidstr, unsigned int hashsize)
{
    const char *str = (const char *) voidstr;
    if (str == NULL) { return 0; } // would be better to have some kind of error
    unsigned int sum = 0;
    for (unsigned int i = 0; str[i] != 0; i++) {
        sum += str[i];
    }
    return sum % hashsize;
}

/*
* Function for comparing string keys or values, for use in hash table
*/
bool comparestr(const void *a, const void *b)
{
    return strcmp(a, b) == 0;
}

/*
* Function for copying string keys or values, for use in hash table
*/
void *copystr(const void *str)
{
    return strdup(str);
}

/*
* Function for freeing string keys or values, for use in hash table
*/
void freestr(void *str)
{
    free(str);
}