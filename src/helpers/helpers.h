#ifndef HELPERS_H
#define HELPERS_H

#define MIN(a,b) ((a) < (b) ? (a) : (b))

#define MAXPORT 65535

int getPort(const char *string, unsigned short *port);
int strlcat3(char *dstStart, char **dstCur, const char *src, unsigned int maxSize);
int getFormattedDate(char *buf, unsigned int size);
int strncpyuntil(char *dest, const char *src, unsigned int length, char delim, char **cur);
//int strlcat2(char *dst, const char *src, int maxSize, unsigned int *curSize);
char strlcat2(char *dst, const char *src, unsigned int maxSize, unsigned int *curSize, unsigned int addSize);

#endif /* HELPERS_H */