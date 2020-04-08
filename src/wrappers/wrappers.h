#ifndef WRAPPERS_H
#define WRAPPERS_H

#include <stdio.h>
#include <sys/socket.h>
#include <signal.h>

//#include "constants.h"

#define CLKID CLOCK_BOOTTIME

// ERRORS:
// 1 and 2 are standard error numbers
#define ARGSERROR 3
#define CREDENTIALSERROR 4
#define BUILDSOCKETERROR 5
#define CLOSESOCKETERROR 6
#define CLOCKERROR 7
#define MALLOCERROR 8
#define THREADERROR 9
#define SEMAPHOREERROR 10
#define SENDERROR 11
#define EVENTFDERROR 12
#define SIGHANDLERERROR 13
#define TIMERFDERROR 14
#define FILEERROR 15

// enum??

#define GETTIME(ts) Clock_gettime(CLOCK_BOOTTIME, ts) // BAD??
// probably just better as a function
void getTime(struct timespec *tp);

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

ssize_t Send(int sockfd, const void *buf, size_t len, int flags);

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

void Inet_pton(int af, const char *src, void *dst);

void *Reallocarray(void *ptr, size_t nmemb, size_t size);

unsigned long Lseek(int fd, off_t offset, int whence);

#endif /* WRAPPERS_H */
