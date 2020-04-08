#include <stdio.h>
//#include <string.h>
#include <time.h>
//#include <error.h>
//#include <signal.h>
//#include <unistd.h>
#include <stdlib.h>
//#include <assert.h>

#include "helpers.h"

// not sure this fits with what helpers.c has become
int getPort(const char *string, unsigned short *port)
{
    long lport = strtol(string, NULL, 0);
    if (lport > MAXPORT || lport < 0) {
        fprintf(stderr, "Provided port number not in range. Legal port numbers 0..%d", MAXPORT);
        return 1;
    }
    *port = (unsigned short) lport;
    return 0;
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
int strncpyuntil(char *dest, const char *src, unsigned int length, char delim, char **cur)
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

// strcmpuntil


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