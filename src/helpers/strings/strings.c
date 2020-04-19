#include <stdlib.h>
#include <string.h>
//#include <stdbool.h>
// what is the c standard for bool
// can you do stuff like bool a = (b > 0)

#include "strings.h"
//#include "bool/bool.h"


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

/* copies byes from src to dest until either length bytes have been copied (including terminating null byte)
 * or src null terminates
 * return 0 on success
 * 1 if delimiter was not found in src before copying ended
 * dest will always be null terminated
 * If cur is not NULL, Will update *cur to point to the next byte after the delimiter
 * If src null terminates within given length, *cur will point to the null byte
*/
int strncpyuntil(char *dest, const char *src, unsigned int length, char delim, const char **cur)
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


/*
* Strcmp, but only true/false for equality, and stops at either null byte or given delimiter
* if not equal, return 0
* if equality test ended at delimiter, 1
* if equality test ended at null byte, 2
* Updates *s1P to point to next byte, or null byte
* So provide your string in s1 and comparison in s2
* Not tested
*/
int strcmpequntil(const char **s1P, const char *s2, char delim)
{
    // better to increment pointers rather than use unsigned int index because with index you have limited capacity
    /*
    for (;! ((*s1 == delim && *s2 == delim) || *s1 == 0 && *s2 == 0); s1++, s2++) {
        if (*s1 != *s2) { return 0;}
    }*/
    // ! null checks

    const char *s1 = *s1P;
    for (;;s1++, s2++) {
        if (*s1 == delim) {
            if (*s2 == delim) { break; }
            return 0;
        }
        if (*s1 == 0) {
            if (*s2 == 0) { *s1P = s1; return 2; }
            return 0;
        }
        if (*s1 != *s2) {
            return 0;
        }
    }
    *s1P = s1+1;
    return 1;
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

/* Takes array of strings to concatenate to dst
 * Array must be finished with a null pointer
 * starts writing from *dstCur and updates *dstCur to next byte to write into (ie to null byte)
 * if dstCur is NULL, starts writing from dstStart and obv doesn't update dstcur
 * 0: success
 * 1: not enough space for src
 * !!!! not tested
*/
int strlcat4(char* dstStart, char** dstCur, const char* const *srces, unsigned int maxSize)
{
    const char* cursrc;
    char* dst;
    if (dstCur != NULL) {
        maxSize = maxSize - (*dstCur - dstStart);
        dst = *dstCur;
    }
    else {
        dst = dstStart;
    }
    
    char* dstEnd = dstStart + maxSize;
    for (; dst < dstEnd-1 && *dst != 0; dst++) {}
    for (unsigned int i = 0; srces[i] != NULL; i++) {
        cursrc = srces[i];
        while (dst < dstEnd-1 && *cursrc != 0) {
            *(dst++) = *(cursrc++);
        }
        if (*cursrc != 0) {
            // failed to fit this one in
            *dst = 0;
            return 1;
        }
    }
    *dst = 0;
    if (dstCur != NULL) {
        *dstCur = dst;
    }
    return  0;
    
    /*
    while (*src != NULL) {
        cursrc = *src;
        char *dstEnd = dst + maxSize;
        for (; dst < dstEnd-1 && *dst != 0; dst++) {}
        while (dst < dstEnd-1 && *cursrc != 0) {
            *(dst++) = *(cursrc++);
        }
        *dst = 0;
        if (*cursrc != 0) {
            return 1;
        }
        src++;
    }*/
}

/*
* Returns pointer to new null byte, or NULL if didn't find end before s terminated
*/
char *terminateAt(char *s, char end) 
{
    if (s == NULL) { return NULL;}
    for (; *s != end && *s != 0; s++) {}
    if (*s == 0) { return NULL; }
	*s = 0;
    return s;
}

/*
* Terminate string at first of given list (string) of options
* Returns pointer to new null byte, or NULL if didn't find end before s terminated
*/
char *terminateAtOpts(char *s, const char *ends, char *found)
{
    if (s == NULL || ends == NULL) { return NULL;}
    for (; *s != 0; s++) {
        for (const char *endcur = ends; *endcur != 0; endcur++) {
            if (*s == *endcur) {
                *s = 0;
                if (found != NULL) { *found = *endcur; }
                return s;
            }
        }
    }
    return NULL;
}

// return pointer to first byte that is not space
char *skipwsp(const char *s)
{
    if (s == NULL) { return NULL; }
    for (; (*s == ' ' || *s == '\t') && *s != 0; s++) {}
    return s;
}

char *stripwsp(char *s)
{
    if (s == NULL) { return NULL; }
    char *end = s + strlen(s) - 1;
    for (; (*end == ' ' || *end == '\t') && *end != 0; end--) {
        //*end = 0;
    }
    *(end+1) = 0;
    return s;
}
