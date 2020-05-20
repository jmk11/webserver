/*
* Hashtable mapping request header label to a function that processes the value
*/

// #include <string.h>
// #include <stdlib.h>

#include "headerfns.h"
#include "requestheaders.h"
#include "../../helpers/hashtable/hashtableG.h"
#include "../../helpers/hashtable/stringhash.h"
#include "../../helpers/strings/strings.h"

static HashTable *ht; // is this so bad
// key is string, value is function pointer

unsigned int hash(const void *voidstr, unsigned int hashsize);
bool compareKey(const void *a, const void *b);
void *copyValue(const void *value);
void freeValue(void *value);
int buildHeaderFnsHT(void);


/*
* Initialise header functions hash table
* Call once at beginning of program
*/
int buildHeaderFnsHT()
{
    ht = htCreate(30, compareKey, hash, copystr, copyValue, freestr, freeValue);
    int res = htAdd(ht, "Host:", (const void*) manageHost);
    if (res != 0) { return 1; }
    res = htAdd(ht, "User-Agent:", (const void*) manageUA);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Accept:", (const void*) manageAccept);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Accept-Language:", (const void*) manageAcceptLanguage);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Accept-Encoding:", (const void*) manageAcceptEncoding);
    if (res != 0) { return 1; }
    res = htAdd(ht, "DNT:", (const void*) manageDNT);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Connection:", (const void*) manageConnection);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Upgrade-Insecure-Requests:", (const void*) manageUIR);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Referer:", (const void*) manageReferer);
    if (res != 0) { return 1; }
    res = htAdd(ht, "If-Modified-Since:", (const void*) manageIfModifiedSince);
    if (res != 0) { return 1; }
    res = htAdd(ht, "Cache-Control:", (const void*) manageCacheControl);
    if (res != 0) { return 1; }

    return 0;
}

/*
* Destroy header functions hash table
* Call once at end of program
*/
void destroyHeaderFnsHT()
{
    htDestroy(ht);
}

/*
* Look up hash table and return function pointer
* This function maybe isn't necessary? if i returned ht instead of making static
*/
char* (*getHeaderFn(const char *str))(requestHeaders*, char*) 
{
    return (char *(*)(requestHeaders*, char *)) htLookup(ht, str);
}

// this part seems like a hack
bool compareKey(const void *a, const void *b)
{
    // needs to be case insensitive
    return strcmpequntil((const char **) &a, b, ':') == 1;
}

/*
* Hash function for string to hash, consdiering ':' or null byte to indicate end of string
* !! is this right or should function be changed
*/
unsigned int hash(const void *voidstr, unsigned int hashsize)
{
    const char *str = (const char *) voidstr;
    if (str == NULL) { return 0; } // would be better to have some kind of error
    unsigned int sum = 0;
    for (unsigned int i = 0; str[i] != 0 && str[i] != ':'; i++) {
        sum += str[i];
    }
    return sum % hashsize;
}

void *copyValue(const void *value)
{
    return value;
    // not sure what to do about const discarding
    // I don't think I need to copy a function pointer?
    // Couldn't change within life of program?
    //const void *copy = malloc(sizeof(void *));
    //memcpy()
    // I'm not sure that I meet the requirements of restrict...
}

void freeValue(void *value)
{
    return;
}
