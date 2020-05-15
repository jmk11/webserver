#ifndef HASH_H
#define HASH_H

#include "../bool/bool.h"

unsigned int stringhash(const void *voidstr, unsigned int hashsize);

bool comparestr(const void *a, const void *b);
void *copystr(const void *str);
void freestr(void *str);

#endif /* HASH_H */
