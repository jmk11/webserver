#include <string.h>
#include <stdlib.h>

#include "hashtable/hashtableG.h"
#include "hashtable/hash.h"
#include "strings/strings.h"

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

//unsigned int hash(const void *, unsigned int)

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

}

void freeKey(void *key)
{
    free(key);
}

void freeValue(void *value)
{
    return;
}

int buildHeaderFnsHT()
{
    ht = htCreate(150, &compareKey, &stringhash, &copyKey, &copyValue, &freeKey, &freeValue);
    int res = htAdd(ht, "html", "text/html; charset=utf-8");
    if (res != 0) { return 1; }
    res = htAdd(ht, "js", "text/javascript"); // text/plain if not referred from my site?
    if (res != 0) { return 1; }
    res = htAdd(ht, "css", "text/css");
    if (res != 0) { return 1; }
    res = htAdd(ht, "txt", "text/plain");
    if (res != 0) { return 1; }

    res = htAdd(ht, "apng", "image/apng");
    if (res != 0) { return 1; }
    res = htAdd(ht, "bmp", "image/bmp");
    if (res != 0) { return 1; }
    res = htAdd(ht, "gif", "image/gif");
    if (res != 0) { return 1; }
    res = htAdd(ht, "ico", "image/x-icon");
    if (res != 0) { return 1; }
    res = htAdd(ht, "cur", "image/x-icon");
    if (res != 0) { return 1; }
    res = htAdd(ht, "jpg", "image/jpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "jpeg", "image/jpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "jfif", "image/jpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "pjpeg", "image/jpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "pjp", "image/jpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "png", "image/png");
    if (res != 0) { return 1; }
    res = htAdd(ht, "svg", "image/svg+xml");
    if (res != 0) { return 1; }
    res = htAdd(ht, "tif", "image/tif");
    if (res != 0) { return 1; }
    res = htAdd(ht, "tiff", "image/tiff");
    if (res != 0) { return 1; }
    res = htAdd(ht, "webp", "image/webp");
    if (res != 0) { return 1; }

    res = htAdd(ht, "mp4", "video/mp4");
    if (res != 0) { return 1; }

    res = htAdd(ht, "rar", "application/x-rar-compressed");
    if (res != 0) { return 1; }
    
    res = htAdd(ht, "default", "text/plain");
    if (res != 0) { return 1; }

    return 0;
}