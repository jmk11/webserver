#ifndef HEADERFNSHT_H
#define HEADERFNSHT_H

#include "headers.h"

int buildHeaderFnsHT(void);
void destroyHeaderFnsHT(void);
char* (*getHeaderFn(char *str))(requestHeaders*, char*);

#endif /* HEADERFNSHT_H */
