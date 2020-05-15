/*
* Hash table mapping file extension to MIME type
*/

#include <string.h>

#include "contenttype.h"
#include "headers.h"
#include "../../helpers/hashtable/hashtableG.h"
#include "../../helpers/hashtable/stringhash.h"

//ContentType contentType = {.build = buildContentTypeHT, .setContentType = setContentType, .destroy = destroyContentTypeHT};

static HashTable *ht;
// string -> string

/*
* Build hash table. Call once at beginning of program.
*/
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
    
    res = htAdd(ht, "mp3", "audio/mpeg");
    if (res != 0) { return 1; }

    res = htAdd(ht, "rar", "application/x-rar-compressed");
    if (res != 0) { return 1; }

    res = htAdd(ht, "json", "application/json");
    if (res != 0) { return 1; }
    
    res = htAdd(ht, "default", "text/plain");
    if (res != 0) { return 1; }

    return 0;
}

/*
* Return content type pointer from hash table
*/
char *setContentType(bool sameDomainReferer, const char *fileExtension)
{
    char *contentTypestr = htLookup(ht, fileExtension);
    if (contentTypestr == NULL) {
        contentTypestr = htLookup(ht, "default");
    }
    return contentTypestr;

    /*
    if (strcmp(fileExtension, "js") == 0) {
        if(sameDomainReferer) { *contentType = "text/javascript"; }
        else { *contentType = "text/plain"; } // I'm assuming these strings go in the data region...
    }*/
    /*if (strcmp(fileExtension, "js") == 0) {
        if (sameDomainReferer) { contentTypestr = htLookup(ht, "js"); }
        else { contentTypestr = htLookup(ht, "txt"); }
    }
    else {*/
    /*}*/
}

/*
* Destroy hash table. Call once at end of program.
*/
void destroyContentTypeHT() {
    htDestroy(ht);
}
