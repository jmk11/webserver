#include <stdlib.h>

#include "hash.h"

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