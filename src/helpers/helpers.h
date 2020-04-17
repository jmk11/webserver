#ifndef HELPERS_H
#define HELPERS_H

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define MAXPORT 65535

int getPort(const char *string, unsigned short *port);
int getHTTPDate(char *buf, unsigned int size);

#endif /* HELPERS_H */
