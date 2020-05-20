/*
* Hash table mapping file extension to MIME type
*/

// #include <string.h>
#include <stddef.h> // for NULL. should import this or something else?

#include "contenttype.h"
// #include "headers.h"
#include "../../helpers/hashtable/hashtableG.h"
#include "../../helpers/hashtable/stringhash.h"

static HashTable *ht;
// string -> string

void *copyromstring(const void *str);
void freeromstring(void *str);


/*
* Build hash table. Call once at beginning of program.
*/
int buildContentTypeHT()
{
    ht = htCreate(150, comparestr, stringhash, copyromstring, copyromstring, freeromstring, freeromstring);
    int res = htAdd(ht, "html", "text/html; charset=utf-8");
    if (res != 0) { return 1; }
    res = htAdd(ht, "js", "text/javascript"); // text/plain if not referred from my site?
    if (res != 0) { return 1; }
    res = htAdd(ht, "css", "text/css");
    if (res != 0) { return 1; }
    res = htAdd(ht, "txt", "text/plain");
    if (res != 0) { return 1; }
    res = htAdd(ht, "htm", "text/html; charset=utf-8");
    if (res != 0) { return 1; }

    res = htAdd(ht, "csv", "text/csv");
    if (res != 0) { return 1; }

    res = htAdd(ht, "apng", "image/apng");
    if (res != 0) { return 1; }
    res = htAdd(ht, "bmp", "image/bmp");
    if (res != 0) { return 1; }
    res = htAdd(ht, "gif", "image/gif");
    if (res != 0) { return 1; }
    res = htAdd(ht, "ico", "image/x-icon"); // image/vnd.microsoft.icon
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
    res = htAdd(ht, "avi", "video/x-msvideo");
    if (res != 0) { return 1; }
    res = htAdd(ht, "mpeg", "video/mpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "ogv", "video/ogg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "webm", "video/webm");
    if (res != 0) { return 1; }
    
    res = htAdd(ht, "mp3", "audio/mpeg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "aac", "audio/aac");
    if (res != 0) { return 1; }
    res = htAdd(ht, "ogg", "audio/ogg");
    if (res != 0) { return 1; }
    res = htAdd(ht, "wav", "audio/wav");
    if (res != 0) { return 1; }
    res = htAdd(ht, "weba", "audio/webm");
    if (res != 0) { return 1; }

    res = htAdd(ht, "rar", "application/x-rar-compressed");
    if (res != 0) { return 1; }
    res = htAdd(ht, "bz", "application/x-bzip");
    if (res != 0) { return 1; }
    res = htAdd(ht, "bz2", "application/x-bzip2");
    if (res != 0) { return 1; }
    res = htAdd(ht, "gz", "application/gzip");
    if (res != 0) { return 1; }
    res = htAdd(ht, "zip", "application/zip");
    if (res != 0) { return 1; }
    res = htAdd(ht, "7zip", "application/x-7z-compressed");
    if (res != 0) { return 1; }

    res = htAdd(ht, "json", "application/json");
    if (res != 0) { return 1; }
    res = htAdd(ht, "jsonld", "application/ld+json");
    if (res != 0) { return 1; }

    res = htAdd(ht, "doc", "application/msword");
    if (res != 0) { return 1; }
    res = htAdd(ht, "docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document");
    if (res != 0) { return 1; }
    res = htAdd(ht, "ppt", "application/vnd.ms-powerpoint");
    if (res != 0) { return 1; }
    res = htAdd(ht, "pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation");
    if (res != 0) { return 1; }
    res = htAdd(ht, "xls", "application/vnd.ms-excel");
    if (res != 0) { return 1; }
    res = htAdd(ht, "xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
    if (res != 0) { return 1; }

    res = htAdd(ht, "odp", "application/vnd.oasis.opendocument.presentation");
    if (res != 0) { return 1; }
    res = htAdd(ht, "ods", "application/vnd.oasis.opendocument.spreadsheet");
    if (res != 0) { return 1; }
    res = htAdd(ht, "odt", "application/vnd.oasis.opendocument.text");
    if (res != 0) { return 1; }

    res = htAdd(ht, "rtf", "application/rtf");
    if (res != 0) { return 1; }

    res = htAdd(ht, "ics", "text/calendar");
    if (res != 0) { return 1; }

    res = htAdd(ht, "pdf", "application/pdf");
    if (res != 0) { return 1; }

    res = htAdd(ht, "", "application/octet-stream");
    if (res != 0) { return 1; }
    res = htAdd(ht, "bin", "application/octet-stream");
    if (res != 0) { return 1; }
    res = htAdd(ht, "jar", "application/java-archive");
    if (res != 0) { return 1; }
    res = htAdd(ht, "sh", "application/x-sh");
    if (res != 0) { return 1; }
    res = htAdd(ht, "swf", "application/x-shockwave-flash");
    if (res != 0) { return 1; }


    res = htAdd(ht, "default", "application/octet-stream");
    if (res != 0) { return 1; }

    return 0;
}

//char *setContentType(bool sameDomainReferer, const char *fileExtension)
/*
* Return content type pointer from hash table
*/
char *setContentType(const char *fileExtension)
{
    char *contentTypestr = htLookup(ht, fileExtension);
    if (contentTypestr == NULL) {
        // maybe let the browser sniff it if type unknown?
        // when does browser decide mime type itself?
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
void destroyContentTypeHT() 
{
    htDestroy(ht);
}

// All the keys and values given to this hash table are in global read only memory, it is a waste of time and memory to copy to the heap
void *copyromstring(const void *str)
{
    return str;
}

void freeromstring(void *str)
{
    return;
}
