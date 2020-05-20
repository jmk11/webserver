/*
* Parse request headers into a requestheaders struct
*/

#include <stdlib.h>
#include <string.h>

#include "requestheaders.h"
#include "headerfns.h"
#include "../helpers.h"
#include "../statuscodes.h"
#include "../../helpers/strings/strings.h"
#include "../../helpers/hashtable/stringhash.h"

char *extractMethod(requestHeaders *headers, const char *headersstr);
char *extractResource(requestHeaders *headers, char *headersstr, char *finish);
char *extractQueryParameters(requestHeaders *headers, char *headersstr);
char *manageVersion(requestHeaders *headers, const char *headersstr);
char *manageCharHeader(char *headersstr, char **resultLocation);
char *uriDecode(const char *headersstr, char **cur);
char *percentDecode(const char *headersstr, char *dest);
char hexcharToNum(char hexchar);

static const struct requestHeaders requestHeadersBase = {
    .method = METHODINIT,
    .resource = NULL,
    .Host = NULL,
    .UserAgent = NULL,
    .Accept = NULL,
    .AcceptLanguage = NULL,
    .AcceptEncoding = NULL,
    .DNT = DNTINIT,
    .ConnectionKeep = TRUE,
    .UpgradeInsecureRequests = UIRINIT,
    .Referer = NULL,
    .IfModifiedSince = -1,
    .CacheControl = NULL,
    .queryParameters = NULL,
    .resourceMalloced = FALSE
};

/*
* Destroy malloced memory placed in requestHeaders struct during parseHeaders()
*/
void freeRequestHeaders(requestHeaders *headers)
{
    if (headers != NULL) {
        if (headers->resourceMalloced == TRUE) {
            free(headers->resource);
        }
        if (headers->queryParameters != NULL) {
            htDestroy(headers->queryParameters);
        }
    }
}

/*
* Parses headersstr and puts headers into headers struct
* On success, returns NULL
* On failure, returns recommended status code to return, as pointer to string in read only memory
* Possible returned status codes: 400, 501, 505
*/
const char *parseHeaders(requestHeaders *headers, char *headersstr)
{
    *headers = requestHeadersBase;

    // parse first line - GET index.html HTTP/1.1
    char finish;
    if ((headersstr = extractMethod(headers, headersstr)) == NULL) { return STATUS_NOTIMPLEMENTED; }
    if ((headersstr = extractResource(headers, headersstr, &finish)) == NULL) { return STATUS_BADREQUEST; }
    if (finish == '?') {
        if ((headersstr = extractQueryParameters(headers, headersstr)) == NULL) { return STATUS_BADREQUEST; }
    }
    if ((headersstr = manageVersion(headers, headersstr)) == NULL) { return STATUS_HTTPVERSIONNOTSUPPORTED; }
    // string literals are statically allocated according to C standard

    // exercise: linked list on stack

    // parse headers
    while (headersstr[0] != 0 && headersstr[0] != '\r') {
        char* (*fn)(requestHeaders*, char*) = getHeaderFn(headersstr);
        if (fn == NULL) {
            // don't know how to parse this header, skip to next
            headersstr = strchr(headersstr, '\r');
            if (headersstr == NULL || *(++headersstr) != '\n') { return STATUS_BADREQUEST; }
            headersstr++;
        }
        else {
            headersstr = strchr(headersstr, ':') + 1;
            headersstr = fn(headers, headersstr);
            if (headersstr == NULL) { return STATUS_BADREQUEST; }
        }
    }
    return NULL;
}

/*
* Given pointer to beginning of method part of headers string,
* set method in headers
* Return pointer to first byte of string after method finishes
* Return NULL on invalid method
*/
char *extractMethod(requestHeaders *headers, const char *headersstr) 
{
    // it turns out that there is a strncasecmp function in strings.h, possibly want to use it
    // probably not though, case matters
    if (strcmpequntil(&headersstr, "GET ", ' ') == 1) {
        headers->method = METHOD_GET;
        return headersstr;
    }
    else if (strcmpequntil(&headersstr, "HEAD ", ' ') == 1) {
        headers->method = METHOD_HEAD;
        return headersstr;
    }
    else {
        return NULL;
        // return 501 not implemented
    }
    // could do with hashtable after replacing first space with null byte, or use strcmpequntil space
}

// request is null terminated within BUFSIZ
// ideally the %20 etc. conversion would be done here
// but atm parseHeaders does not return any malloced memory
// it would be nice to keep it that way
// so I don't have to track what strings are malloced and which aren't
// but it really doesn't make sense for this conversion to be anywhere else
/*
 * edits headerstr to null terminate resource, and sets headers struct to point to resource
 * Sets finish to the character that was replaced with the null byte
 * Returns pointer to next byte of string after resource
 * Returns NULL on malformed request
*/
char *extractResource(requestHeaders *headers, char *headersstr, char *finish)
{
    if (headersstr[0] != '/') {
        return NULL;
    }
    headers->resource = headersstr+1;
    char *cur;
    char *resource = uriDecode(headersstr+1, &cur);
    if (*cur == 0) { return NULL; }
    if (resource != NULL) {
        headers->resource = resource;
        headers->resourceMalloced = TRUE;
    }
    // resource ends at either space or ?, indicating start of query parameters
   /* char *cur = strpbrk(headers->resource, " ?");
    if (cur == NULL) { return NULL; }*/
    *finish = *cur;
    *cur = 0;
    // strprbk vs terminateAtOpts
    return cur + 1;
}

/*
* Decode percent encoding. If there is none, return NULL.
* If there is, return pointer to malloced decoded string.
* Set *cur to address of first space or ? in headersstr, or ***
*/
char *uriDecode(const char *headersstr, char **cur)
{
    bool decode = FALSE;
    unsigned int decodestart;
    unsigned int i;
    for (i = 0; headersstr[i] != ' ' && headersstr[i] != '?' && headersstr[i] != 0; i++) {
        if (headersstr[i] == '%' && decode == FALSE) {
            decode = TRUE;
            decodestart = i;
        }
    }
    if (decode) {
        char *decoded = malloc(i+1);
        strncpy(decoded, headersstr, decodestart);
        *cur = percentDecode(&headersstr[decodestart], decoded+decodestart);
        // if failed: ??
        return decoded;
    }
    *cur = &headersstr[i];
    return NULL;
}

char *percentDecode(const char *headersstr, char *dest) 
{
    for (; *headersstr != ' ' && *headersstr != '?' && *headersstr != 0; headersstr++) {
        if (*headersstr == '%') {
            char hexdigit1 = *(headersstr+1);
            char hexdigit2 = *(headersstr+2);
            char hexdigit1char = hexcharToNum(hexdigit1);
            if (hexdigit1char != -1) {
                char hexdigit2char = hexcharToNum(hexdigit2);
                if (hexdigit2char != -1) {
                    char val = hexdigit1char << 4 | hexdigit2char;
                    *(dest++) = val;
                    headersstr = headersstr + 2;
                    continue;
                }
            }
        }
        *(dest++) = *headersstr;
        /*
            if (hexdigit1char == -1) {
                goto COPY; // copy
            }
            char hexdigit2char = hexcharToNum(hexdigit2);
            if (hexdigit2char == -1) {
                goto COPY; // copy
            }
            char val = hexdigit1char << 4 | hexdigit2char;
            *(dest++) = val;
            headersstr = headersstr + 3;
        }
        else {
            COPY:
            *(dest++) = *headersstr;
        }
        */
    }
    return headersstr;
}

char hexcharToNum(char hexchar)
{
    if (hexchar >= '0' && hexchar <= '9') {
        return hexchar - '0';
    }
    if (hexchar >= 'A' && hexchar <= 'F') {
        return 10 + hexchar - 'A';
    }
    if (hexchar >= 'a' && hexchar <= 'f') {
        return 10 + hexchar - 'A';
    }
    // !! what is the specification - uppercase or either?
    else {
        return -1;
    }
}

/*
* Create query parameters hash table
* Hash table needs to be destroyed later
* Returns pointer to next byte of string after resource
* Returns NULL on malformed request. If so, hash table has been cleaned up.
* Not tested // TODO
*/
char *extractQueryParameters(requestHeaders *headers, char *headersstr)
{
    headers->queryParameters = htCreate(30, comparestr, stringhash, strdup, strdup, free, free);
    char *cur = headersstr;
    char *label, *value;
    char finish = 0;
    while (finish != ' ') {
        label = cur;
        cur = terminateAt(label, '=');
        if (cur == NULL) { htDestroy(headers->queryParameters); return NULL; }
        value = cur;
        cur = terminateAtOpts(value, "& ", &finish);
        if (cur == NULL) { htDestroy(headers->queryParameters); return NULL; }
        int res = htAdd(headers->queryParameters, label, value);
        if (res != 0) { htDestroy(headers->queryParameters); return NULL; }
        cur = cur + 1;
        // would be faster if made own compare function to add an equals to key if there isn't one
    }
    return cur;
}

/*
* Returns pointer to next byte of string after resource
* Returns NULL on version not HTTP/1.x, or malformed request
*/
char *manageVersion(requestHeaders *headers, const char *headersstr)
{
    // HTTP/1.0: no persistent connection
    // HTTP/1.1: keep connection by default

    if (! (strcmpequntil(&headersstr, "HTTP/1.", '.'))) {
        return NULL; // 505
    }
    switch (headersstr[0]) {
        case '0': headers->ConnectionKeep = FALSE; break;
        case '1': headers->ConnectionKeep = TRUE; break;
        default: return NULL; // 505
    }

    /*
    if (headersstr[0] == '0') {
        headers->ConnectionKeep = FALSE;
    }
    else if (headersstr[0] == '1') {
        headers->ConnectionKeep = TRUE;
    }
    else {
        return NULL;
        // return 505 HTTP version not supported
    }
    */

    if (headersstr[1] != '\r' || headersstr[2] != '\n') {
        return NULL; // 400
    }
     
    return headersstr + 3;

    // && (strcmpequntil(&(headersstr[1]), "\r"))) // does &(headersstr[1]) have 

    /*
    if (! (
        (strcmpequntil(&headersstr, "HTTP/1", '.'))
        && (headersstr[0] == '0' || headersstr[0] == '1')
        && (headersstr[1] == '\r' && headersstr[2] == '\n')
        )) {
        return 1;
    }*/
}

// I think header values are allowed to have any amount of whitespace (space | horizontal tab) before and after them
// change
// https://www.w3.org/TR/upgrade-insecure-requests/#preference
// TODO

/* char *manageHeader (requestHeaders *headers, char *headersstr) interface:
 * Takes pointer to value in headers string
 * Sets value in headers struct as appropriate
 * Returns pointer to first byte of next header, or NULL on malformed request
*/

// resultLocation name
/*
* For headers that simply set a pointer to the value in the requestheaders struct
*/
char *manageCharHeader(char *headersstr, char **resultLocation)
{
    headersstr = skipwsp(headersstr);
    // terminate string for requestheaders, and check request syntax
    char *cur = terminateAt(headersstr, '\r');
    // I want to move to \r, but if there is whitespace, terminate at first whitespace
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    *resultLocation = headersstr;
    return cur + 1;
}

char *manageHost(requestHeaders *headers, char *headersstr)
{
    return manageCharHeader(headersstr, &headers->Host);
}

char *manageUA(requestHeaders *headers, char *headersstr)
{
    return manageCharHeader(headersstr, &headers->UserAgent);
}

char *manageAccept(requestHeaders *headers, char *headersstr)
{
    return manageCharHeader(headersstr, &headers->Accept);
}

char *manageAcceptLanguage(requestHeaders *headers, char *headersstr)
{
    return manageCharHeader(headersstr, &headers->AcceptLanguage);
}

char *manageAcceptEncoding(requestHeaders *headers, char *headersstr)
{
    return manageCharHeader(headersstr, &headers->AcceptEncoding);
}

char *manageCacheControl(requestHeaders *headers, char *headersstr)
{
    return manageCharHeader(headersstr, &headers->CacheControl);
}

char *manageDNT(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    if (headersstr[0] == '0') { headers->DNT = DNT0; }
    else if (headersstr[0] == '1') { headers->DNT = DNT1; }
    else { return NULL; }

    char *cur = strchr(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    return cur + 1;
}

char *manageConnection(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = headersstr;
    if (strcmpequntil(&cur, "keep-alive\r", '\r') == 1) { 
        headers->ConnectionKeep = TRUE; 
        // if returns 2, malformed request. Return here?
    }
    else {
        cur = headersstr;
        if (strcmpequntil(&cur, "close\r", '\r') == 1) {
            // strcmpequntil cannot possibly return 2 here because s2 has the delim // ??
            headers->ConnectionKeep = FALSE; 
        }
        else {
            return NULL;
        }
    }
    
    if (*cur != '\n') { return NULL; }
    return cur + 1;
}

char *manageUIR(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    if (headersstr[0] == '1') { headers->UpgradeInsecureRequests = UIR1; }
    else { return NULL; }

    char *cur = strchr(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    return cur + 1;
}

char *manageReferer(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->AcceptLanguage = headersstr;
    return cur + 1;
    // store if same domain or do elsewhere?
}

char *manageIfModifiedSince(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    time_t mstime = HTTPDatetotime(headersstr);
    if (mstime < 0) { return NULL; }
    headers->IfModifiedSince = mstime;
    return cur + 1;
}
