#ifndef STRINGS_H
#define STRINGS_H

int strncpyuntil(char *dest, const char *src, unsigned int length, char delim, char **cur);
//int strlcat2(char *dst, const char *src, int maxSize, unsigned int *curSize);
char strlcat2(char *dst, const char *src, unsigned int maxSize, unsigned int *curSize, unsigned int addSize);
int strcmpequntil(const char *s1, const char *s2, char delim);
int strlcat3(char *dstStart, char **dstCur, const char *src, unsigned int maxSize);

#endif /* STRINGS_H */
