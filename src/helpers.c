#include <string.h>
#include <time.h>
#include <pthread.h>
#include <error.h>
//#include <sys/eventfd.h>
//#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
//#include <assert.h>
#include <arpa/inet.h>
#include <malloc.h>
//#include <sys/timerfd.h>

#include "constants.h"
#include "helpers.h"

// not sure this fits with what helpers.c has become
unsigned short getPort(const char *string)
{
    long lport = strtol(string, NULL, 0);
    if (lport > MAXPORT || lport < 0) {
        printf("Provided port number not in range. Legal port numbers 0..%d", MAXPORT);
        exit(ARGSERROR);
    }
    return (unsigned short) lport;
}

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

// 0: success
// 1: not enough space for src
// !!!! not tested
int strlcat3(char *dstStart, char **dstCur, const char *src, unsigned int maxSize)
{
    maxSize = maxSize - (*dstCur - dstStart);
    char *dst = *dstCur;
    char *dstEnd = dst + maxSize;
    for (; dst < dstEnd-1 && *dst != 0; dst++) {}
    while (dst < dstEnd-1 && *src != 0) {
        *(dst++) = *(src++);
    }
    *dst = 0;
    *dstCur = dst;
    if (*src != 0) {
        return 1;
    }
    return 0;
}


// THIS COMMENT IS FOR AN OLD VERSION
// Always null-terminates UNLESS 2
// Final string size will be AT MOST n bytes, INCLUDING null byte
// NAME
/*
 * This description is old
 * Starts writing at dst+curSize
 * Idea is that the current null byte is at dst+curSize
 * Copies from src until src is null terminated or address (dst+maxSize-1) is reached, whichever is sooner
 * Null terminate at dst+maxSize-1
 * Will always null terminate dst (unless (2)) and sets curSize to the total length of dst, not including the null byte
 * curSize will equal the given curSize + the number of bytes written, excluding terminating null byte
 * addSize, if not 0, is the number of bytes to add. If 0, will add until null byte is found (or run out of space etc.)
 * RETURNS:
 * 0 if all goes correctly
 * 1 if there was not enough space for src - ie copying was terminated before src's null byte was found, or before addSize was exhausted
 * 2 if the provided curSize is greater than or equal to the provided maxSize. In this case, no changes were made to any provided memory.
 */
char strlcat2(char *dst, const char *src, unsigned int maxSize, unsigned int *curSize, unsigned int addSize)
{
    if (*curSize >= maxSize) { return 2; }

    /*bool maxSizeChanged = FALSE; // this is stupid
    if (addSize != 0 && (*curSize + addSize + 1 <= maxSize)) {
        maxSize = *curSize + addSize + 1; // +1 because maxSize includes null byte while others do not.
        maxSizeChanged = TRUE;
    }*/

    //if (addSize == 0) { addSize = maxSize - *curSize - 1; }
    //if (addSize == 0) { addSize = maxSize; } // makes very big... takes out of equation... seems bad
    // I should be able to change this somehow so only need addSize in if
    // and prob something else to check if terminated copying because of addSize or because of original maxSize ...

    // !! test this bad new stuff
    char *start = dst + *curSize;
    char *cur = start; //dst + *curSize;
    while ((cur < dst + maxSize - 1) && ((addSize == 0 && *src != 0) || (addSize != 0 && cur < start + addSize))) {
    //while ((*src != 0) && (cur < dst + (maxSize - 1)) && !(addSize != 0 && cur >= start + addSize)) { // or do as: if curSize < maxSize - 1
        *(cur++) = *(src++);
        (*curSize)++;
    }
    *cur = 0;
    //*curSize = cur - dst - 1;
    //if (*src != 0 && (cur != dst + (maxSize - 1))) {
    if (cur == dst + (maxSize - 1) && !(addSize != 0 && cur == start + addSize)) {
        // copying ended only because reached maxSize
        return 1;
    }

    return 0;
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

int getFormattedDate(char *buf, unsigned int size)
{
    time_t t = time(NULL);
    if (t == -1) {
        perror("time() failed");
        return 1;
    }
	struct tm *tm = gmtime(&t);
    if (tm == NULL) {
        perror("gmtime() failed");
        return 1;
    }
	size_t bytesWritten = strftime(buf, size, "%a, %d %b %Y %H:%M:%S %Z", tm);
    if (bytesWritten <= 0) {
        return 1;
    }
    return 0;
}

/* copies byes from src to dest until either length bytes have been copied (including terminating null byte)
 * or src null terminates
 * return 0 on success
 * 1 if delimiter was not found in src before copying ended
 * dest will always be null terminated
 * If cur is not NULL, Will update *cur to point to the next byte after the delimiter
 * If src null terminates within given length, *cur will point to the null byte
*/
int strncpyuntil(char *dest, const char *src, int length, char delim, char **cur)
{
    unsigned int i = 0;
    while (src[i] != 0 && src[i] != delim && i < length-1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    if (src[i] != delim) {
        return 1;
    }
    if (cur != NULL) {
        if (src[i] == 0) {
            *cur = &(src[i]);
        }
        else {
            *cur = &(src[i+1]);
        }
    }

    return 0;
}
