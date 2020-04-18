#ifndef HELPERS_H
#define HELPERS_H

#include <time.h>

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define MAXPORT 65535

int getPort(const char *string, unsigned short *port);
int getHTTPDate(char *buf, unsigned int size);
int timetoHTTPDate(time_t time, char *buf, unsigned int size);
time_t HTTPDatetotime(const char *headersstr);

#endif /* HELPERS_H */
