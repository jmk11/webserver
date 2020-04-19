#include <string.h>
#include <time.h>
#include <pthread.h>
#include <error.h>
//#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
//#include <assert.h>
#include <arpa/inet.h>
#include <malloc.h>

#include "wrappers.h"

int Socket(int domain, int type, int protocol)
{
    int sockfd = socket(domain, type, protocol); // -1 on failure
    if (sockfd < 0) {
        perror("Error: socket");
        exit(BUILDSOCKETERROR);
    }
    return sockfd;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int success = bind(sockfd, addr, addrlen);
    if (success < 0) {
        perror("Error: bind");
        exit(BUILDSOCKETERROR);
    }
}

void Listen(int sockfd, int backlog)
{
    int success = listen(sockfd, backlog);
    if (success < 0) {
        perror("Error: listen");
        exit(BUILDSOCKETERROR);
    }
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int newSockfd = accept(sockfd, addr, addrlen);
    if (newSockfd < 0) {
        perror("Error: accept");
        exit(BUILDSOCKETERROR);
    }
    return newSockfd;
}

void Connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
    int success = connect(sockfd, addr, addrlen);
    if (success < 0) {
        perror("Error: Could not connect to server");
        exit(BUILDSOCKETERROR);
    }
}

// mainly here because I didn't want to #include in client.c
void Close(int fd)
{
    int success = close(fd);
    if (success < 0) {
        perror("Error: close");
        exit(CLOSESOCKETERROR);
    }
}

FILE *Fopen(const char *pathname, const char *mode)
{
    FILE *fp = fopen(pathname, mode);
    if (fp == NULL) {
        char errorMessage[100];
        snprintf(errorMessage, 100, "Error: Can't open file %s", pathname);
        perror(errorMessage); // dont' want to compile this info into client.
        exit(FILEERROR);
    }
    return fp;
}

char *Fgets(char *s, int size, FILE *stream)
{
    char *res = fgets(s, size, stream);
    if (res == NULL) { return NULL; } // !! deal with this somewhere
    //password[strlen(password)-1] = 0;
    char *end = strchr(res, '\r');
    if (end != NULL) { *end = 0; }
    end = strchr(res, '\n');
    if (end != NULL) { *end = 0; }
    return res;
}

void Clock_gettime(clockid_t clk_id, struct timespec *tp)
{
    int success = clock_gettime(clk_id, tp);
    if (success < 0) {
        perror("Error: can't get time");
        exit(CLOCKERROR);
    }
}

// to standardise clk_id
// now using CLKID constant instead?
// since there were time I had to use clkid different to this
void getTime(struct timespec *tp)
{
    int success = clock_gettime(CLOCK_BOOTTIME, tp);
    if (success < 0) {
        perror("Error: can't get time");
        exit(CLOCKERROR);
    }
}

void *Malloc(size_t size)
{
    void *mem = malloc(size);
    if (mem == NULL) {
        printf("Error: malloc memory unavailable, or 0 bytes requested\n");
        exit(MALLOCERROR);
    }
    memset(mem, 0, size);
    return mem;
}

void *Reallocarray(void *ptr, size_t nmemb, size_t size)
{
    void *mem = reallocarray(ptr, nmemb, size);
    if (mem == NULL) {
        printf("Error: realloc memory unavailable, or 0 bytes requested, or multiplication would overflow\n");
        exit(MALLOCERROR);
    }
    return mem;
}

ssize_t Send(int sockfd, const void *buf, size_t len, int flags)
{
    ssize_t result = send(sockfd, buf, len, flags);
    if (result < 0) {
        perror("Error: When sending data");
        exit(SENDERROR);
    }
    return result;
}

void Sigaction(int signum, const struct sigaction *act, struct sigaction *oldact)
{
    int success = sigaction(signum, act, oldact);
    if (success != 0) {
        perror("Error: in setting sighandler");
        exit(SIGHANDLERERROR);
    }
}

void GetSockName(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
    int success = getsockname(sockfd, addr, addrlen);
    if (success != 0) {
        perror("Error: in getting socket details");
        exit(BUILDSOCKETERROR);
    }
}

void Inet_pton(int af, const char *src, void *dst)
{
    int success = inet_pton(af, src, dst);
    if (success != 1) {
        printf("Error: Provided HostIP is invalid.\n");
        exit(ARGSERROR);
    }
}

unsigned long Lseek(int fd, off_t offset, int whence)
{
    long result = lseek(fd, offset, whence);
    if (result < 0) {
        perror("Error: in lseeking");
        exit(FILEERROR);
    }
    return result;
}
