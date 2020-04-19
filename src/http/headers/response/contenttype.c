#include <string.h>

#include "contenttype.h"
#include "headers.h"
#include "../../../helpers/hashtable/hashtableG.h"
#include "../../../helpers/hashtable/hash.h"

int buildContentTypeHT(void);
char *setContentType(bool sameDomainReferer, const char *fileExtension);
void destroyContentTypeHT(void);
ContentType contentType = {.build = buildContentTypeHT, .setContentType = setContentType, .destroy = destroyContentTypeHT};

static HashTable *ht;

bool comparestr(const void *a, const void *b);
void *copystr(const void *str);
void freestr(void *str);

bool comparestr(const void *a, const void *b)
{
    return strcmp(a, b) == 0;
}

void *copystr(const void *str)
{
    return strdup(str);
}

void freestr(void *str)
{
    free(str);
}

int buildContentTypeHT()
{
    ht = htCreate(150, comparestr, stringhash, copystr, copystr, freestr, freestr);
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

    res = htAdd(ht, "json", "application/json");
    if (res != 0) { return 1; }
    
    res = htAdd(ht, "default", "text/plain");
    if (res != 0) { return 1; }

    return 0;
}

char *setContentType(bool sameDomainReferer, const char *fileExtension)
{
    char *contentTypestr;
    /*
    if (contentType == NULL) {
        return 1;
    }*/
    /*
    if (strcmp(fileExtension, "js") == 0) {
        if(sameDomainReferer) { *contentType = "text/javascript"; }
        else { *contentType = "text/plain"; } // I'm assuming these strings go in the data region...
    }*/
    if (strcmp(fileExtension, "js") == 0) {
        if (sameDomainReferer) { contentTypestr = htLookup(ht, "js"); }
        else { contentTypestr = htLookup(ht, "txt"); }
    }
    else {
        contentTypestr = htLookup(ht, fileExtension);
        if (contentTypestr == NULL) {
            contentTypestr = htLookup(ht, "default");
        }
    }
    return contentTypestr;
}

void destroyContentTypeHT() {
    htDestroy(ht);
}
