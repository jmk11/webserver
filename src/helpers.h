#ifndef PROJECT_HELPERS_H
#define PROJECT_HELPERS_H


#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>

//#include "constants.h"

#define GETTIME(ts) Clock_gettime(CLOCK_BOOTTIME, ts) // BAD??
// probably just better as a function
void getTime(struct timespec *tp);

#define MIN(a,b) ((a) < (b) ? (a) : (b))

unsigned short getPort(const char *string);

int Socket(int domain, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void Listen(int sockfd, int backlog);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
void GetSockName(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
void Close(int fd);

FILE *Fopen(const char *pathname, const char *mode);
char *Fgets(char *s, int size, FILE *stream);
void Clock_gettime(clockid_t clk_id, struct timespec *tp);
void *Malloc(size_t size);

//int strlcat2(char *dst, const char *src, int maxSize, unsigned int *curSize);
char strlcat2(char *dst, const char *src, unsigned int maxSize, unsigned int *curSize, unsigned int addSize);

ssize_t Send(int sockfd, const void *buf, size_t len, int flags);

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

void Inet_pton(int af, const char *src, void *dst);

void *Reallocarray(void *ptr, size_t nmemb, size_t size);

unsigned long Lseek(int fd, off_t offset, int whence);

int strlcat3(char *dstStart, char **dstCur, const char *src, unsigned int maxSize);
int strncpyuntil(char *dest, const char *src, int length, char delim, char **cur);

int getFormattedDate(char *buf, unsigned int size);

#endif //PROJECT_HELPERS_H
