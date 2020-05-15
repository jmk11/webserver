#ifndef HEADERFNSHT_H
#define HEADERFNSHT_H

#include "requestheaders.h"

int buildHeaderFnsHT(void);
void destroyHeaderFnsHT(void);
char* (*getHeaderFn(const char *str))(requestHeaders*, char*);

#endif /* HEADERFNSHT_H */
