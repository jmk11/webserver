#ifndef STRINGS_H
#define STRINGS_H

int strncpyuntil(char *dest, const char *src, unsigned int length, char delim, const char **cur);
// int strlcat2(char *dst, const char *src, int maxSize, unsigned int *curSize);
// char strlcat2(char *dst, const char *src, unsigned int maxSize, unsigned int *curSize, unsigned int addSize);
int strcmpequntil(const char **s1, const char *s2, char delim);
int strlcat3(char *dstStart, char **dstCur, const char *src, unsigned int maxSize);
int strlcat4(char *dst, char **dstcur, const char* const *srces, unsigned int maxSize);
char *terminateAt(char *s, char end);
char *terminateAtOpts(char *s, const char *ends, char *found);
char *skipwsp(const char *s);
char *stripwsp(char *s);
int strlcat1(char *dst, const char *src, unsigned int maxSize);
int strlcpy(char *dst, const char *src, size_t length);
char *strlcat5(char *dst, const char *src, unsigned int maxSize);

#endif /* STRINGS_H */
