#include <string.h>

#include "hashtable/hashtable.h"
#include "headers.h"

static HashTable *ht;

int buildContentTypeHT()
{
    ht = htCreate(150);
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

int setContentType(bool sameDomainReferer, const char *fileExtension, char **contentType)
{
    if (contentType == NULL) {
        return 1;
    }
    /*
    if (strcmp(fileExtension, "js") == 0) {
        if(sameDomainReferer) { *contentType = "text/javascript"; }
        else { *contentType = "text/plain"; } // I'm assuming these strings go in the data region...
    }*/
    if (strcmp(fileExtension, "js") == 0) {
        if (sameDomainReferer) { *contentType = htLookup(ht, "js"); }
        else { *contentType = htLookup(ht, "txt"); }
    }
    else {
        *contentType = htLookup(ht, fileExtension);
        if (*contentType == NULL) {
            *contentType = htLookup(ht, "default");
        }
    }
    return 0;
}

void destroyContentTypeHT() {
    htDestroy(ht);
}
