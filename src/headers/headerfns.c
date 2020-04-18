#include <string.h>
#include <stdlib.h>

#include "headerfns.h"
#include "hashtable/hashtableG.h"
#include "hashtable/hash.h"
#include "strings/strings.h"
//#include "headerpriv.h"

static HashTable *ht;
// key is string
// value is function pointer
// void 
//typedef void (*valueFn) (const char *);

// compareKey
// hash
// copykey
// copyvalue
// freekey
// freevalue

bool compareKey(const void *a, const void *b);
void *copyKey(const void *key);
void *copyValue(const void *value);
void freeKey(void *key);
void freeValue(void *value);
int buildHeaderFnsHT(void);

bool compareKey(const void *a, const void *b)
{
    return strcmpequntil((const char **) &a, b, ':') == 1;
}

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

void *copyKey(const void *key)
{
    return strdup(key);
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

void freeKey(void *key)
{
    free(key);
}

void freeValue(void *value)
{
    return;
}

/*
const struct requestHeaders requestHeadersBase = {
    .method = METHODINIT,
    .resource = NULL,
    .Host = {"Host", NULL},
    .UserAgent = {"User-Agent", NULL},
    .Accept = {"Accept", NULL},
    .AcceptLanguage = {"Accept-Language", NULL},
    .AcceptEncoding = {"Accept-Encoding", NULL},
    .DNT = {"DNT", TRUE},
    .ConnectionKeep = {"Connection", TRUE},
    .UpgradeInsecureRequests = {"Upgrade-Insecure-Requests", FALSE},
    .Referer = {"Referer", NULL}
};
*/

int buildHeaderFnsHT()
{
    ht = htCreate(30, compareKey, hash, copyKey, copyValue, freeKey, freeValue);
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

    return 0;
}

void destroyHeaderFnsHT()
{
    htDestroy(ht);
}

char* (*getHeaderFn(const char *str))(requestHeaders*, char*) 
{
    return (char *(*)(requestHeaders*, char *)) htLookup(ht, str);
}

