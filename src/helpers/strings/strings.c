/*
* Helpers for strings
*/

// #include <stdlib.h>
#include <string.h>
//#include <stdbool.h>
// what is the c standard for bool
// can you do stuff like bool a = (b > 0)

#include "strings.h"
//#include "bool/bool.h"

/*
* Concatenate src to dst, such that length of string at dst (including null byte) does not exceed maxSize
* Always null terminates dst
* On success, return 0
* If src did not finish before dst ran out of space, return 1
* !!! not tested
*/
int strlcat1(char *dst, const char *src, unsigned int maxSize)
{
    char *dstEnd = dst + maxSize;
    // find end of dst
    for (; dst < dstEnd-1 && *dst != 0; dst++) {}
    // copy over
    while (dst < dstEnd-1 && *src != 0) {
        *(dst++) = *(src++);
    }
    *dst = 0;
    if (*src != 0) {
        return 1;
    }
    return 0;
}

/*
* Concatenate src to dst, such that length of string at dst (including null byte) does not exceed maxSize
* Always null terminates dst
* On success, return dst
* If src did not finish before dst ran out of space, return NULL
* !!! not tested
*/
char *strlcat5(char *dst, const char *src, unsigned int maxSize)
{
    char *dstEnd = dst + maxSize;
    // find end of dst
    char *cur = dst;
    for (; cur < dstEnd-1 && *cur != 0; cur++) {}
    // copy over
    while (cur < dstEnd-1 && *src != 0) {
        *(cur++) = *(src++);
    }
    *cur = 0;
    if (*src != 0) {
        return NULL;
    }
    return dst;
}


/*
* Concatenate src to dst, starting at *dstCur
* Will concatenate max maxSize bytes from dstStart
* Sets *dstCur to current destination
* Always null terminates src
* Returns 0 on success, 1 if src did not null terminate before space ran out
* !!!! not tested
*/
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

/* 
 * copies bytes from src to dest, until either length bytes have been copied (including terminating null byte),
 * src hits a delim character,
 * or src null terminates.
 * dest will always be null terminated
 * If cur is not NULL, Will update *cur to point to the next byte after the delimiter in src
 * If src null terminates within given length, *cur will point to the null byte
 * return 0 on success
 * return 1 if delimiter was not found in src before copying ended
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
* Updates *s1P to point to next byte, or null byte
* So provide your string in s1 and comparison in s2
* if not equal, return 0
* if equal and equality test ended at delimiter, return 1
* if equal and equality test ended at null byte, return 2
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

/* 
 * Takes array of strings to copy to dst
 * Array must be finished with a null pointer
 * Like {"this ", "is ", "a ", "string", NULL}
 * starts writing from *dstCur and updates *dstCur to next byte to write into (ie to null byte)
 * if dstCur is NULL, starts writing from dstStart and obv doesn't update dstcur
 * return 0: success
 * return 1: not enough space for srces
 * !!!! not tested
*/
int strlcat4(char* dstStart, char** dstCur, const char* const *srces, unsigned int maxSize)
{
    // set dst
    char* dst;
    if (dstCur != NULL) {
        maxSize = maxSize - (*dstCur - dstStart);
        dst = *dstCur;
    }
    else {
        dst = dstStart;
    }
    
    // find current end of dst
    char* dstEnd = dstStart + maxSize;
    for (; dst < dstEnd-1 && *dst != 0; dst++) {}
    
    // copy srces
    const char* cursrc;
    for (unsigned int i = 0; srces[i] != NULL; i++) {
        // unsigned int i overflow - infinite loop if isn't NULL terminated - or until segfault
        // is unsigned int overflow the one that's undefined in the C standard or is it signed int?
        // why am I not doing the way below with pointer arithmetic
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
* Null terminates string at first occurence of character end
* Returns pointer to new null byte, or NULL if didn't find end before s terminated
*/
char *terminateAt(char *s, char end) 
{
    //if (s == NULL) { return NULL;} // don't provide NULL
    for (; *s != end && *s != 0; s++) {}
    if (*s == 0) { return NULL; }
	*s = 0;
    return s;
}

/*
* Null terminate string at first of given list (string) of options
* Returns pointer to new null byte, or NULL if didn't find any of ends before s terminated
* If one of ends was found, sets *found to the value found
*/
char *terminateAtOpts(char *s, const char *ends, char *found)
{
    //if (s == NULL || ends == NULL) { return NULL;} // don't provide NULL
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

/* 
* Returns pointer to first byte in string that is not space or tab
* If no whitespace is found before null termination, returns pointer to null byte
*/
char *skipwsp(const char *s)
{
    // if (s == NULL) { return NULL; }
    for (; (*s == ' ' || *s == '\t') && *s != 0; s++) {}
    return s;
}

/*
* Strips whitespace from right of string, by replacing beginning of end-of-string whitespace with null byte
* Returns s
*/
char *stripwsp(char *s)
{
    // if (s == NULL) { return NULL; }
    char *end = s + strlen(s) - 1;
    for (; (*end == ' ' || *end == '\t') && *end != 0; end--) {
        //*end = 0;
    }
    *(end+1) = 0;
    return s;
}

/*
* Copy length bytes from src to dst
* Return 0 if string src terminated within length bytes, 1 if not
* Always null terminates dst within length bytes
*/
int strlcpy(char *dst, const char *src, size_t length) {
    char *dstend = dst+length;
    while (dst < dstend-1 && *src != 0) {
        *(dst++) = *(src++);
    }
    *dst = 0;
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
/*
char strlcat2(char *dst, const char *src, unsigned int maxSize, unsigned int *curSize, unsigned int addSize)
{
    if (*curSize >= maxSize) { return 2; }

    // bool maxSizeChanged = FALSE; // this is stupid
    // if (addSize != 0 && (*curSize + addSize + 1 <= maxSize)) {
    //     maxSize = *curSize + addSize + 1; // +1 because maxSize includes null byte while others do not.
    //     maxSizeChanged = TRUE;
    // }

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
    // *curSize = cur - dst - 1;
    //if (*src != 0 && (cur != dst + (maxSize - 1))) {
    if (cur == dst + (maxSize - 1) && !(addSize != 0 && cur == start + addSize)) {
        // copying ended only because reached maxSize
        return 1;
    }

    return 0;
}
*/
