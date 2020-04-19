#ifndef HELPERS_H
#define HELPERS_H

#include <time.h>

int getHTTPDate(char *buf, unsigned int size);
int timetoHTTPDate(time_t time, char *buf, unsigned int size);
time_t HTTPDatetotime(const char *headersstr);

#endif /* HELPERS_H */
