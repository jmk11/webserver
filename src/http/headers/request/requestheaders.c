#include <stdlib.h>
#include <string.h>

#include "requestheaders.h"
#include "requestheadersbase.h"
#include "headerfns.h"
#include "../helpers.h"
#include "../../statuscodes.h"
#include "../../../helpers/strings/strings.h"
#include "../../../helpers/hashtable/hash.h"

char *extractMethod(requestHeaders *headers, const char *headersstr);
char *extractResource(requestHeaders *headers, char *headersstr, char *finish);
char *extractQueryParameters(requestHeaders *headers, char *headersstr);
char *manageVersion(requestHeaders *headers, const char *headersstr);


int initialiseRequestHeaders(requestHeaders *headers) 
{
    *headers = requestHeadersBase;
    return 0;
}

void freeRequestHeaders(requestHeaders *headers)
{
    if (headers != NULL && headers->queryParameters != NULL) {
        free(headers->queryParameters);
    }
}

char *extractMethod(requestHeaders *headers, const char *headersstr) 
{
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
    // could do with hashtable after replacing first space with null byte
}

// request is null terminated within BUFSIZ
// edits request to null terminate filename
// and sets pointer to where filename starts
char *extractResource(requestHeaders *headers, char *headersstr, char *finish)
{
    if (headersstr[0] != '/') {
        return NULL;
    }
    headers->resource = headersstr+1;
    // resource should also end at ? and take query parameters
    // char *cur = terminateAt(headers->resource, ' ');
    char *cur = strpbrk(headers->resource, " ?");
    if (cur == NULL) { return NULL; }
    *finish = *cur;
    *cur = 0;
    // strprbk vs terminateAtOpts
    return cur + 1;
}


char *extractQueryParameters(requestHeaders *headers, char *headersstr)
{
    headers->queryParameters = htCreate(30, strcmp, stringhash, strdup, strdup, free, free);
    char *cur = headersstr;
    char *label, *value;
    char finish = 0;
    while (finish != ' ') {
        label = cur;
        cur = terminateAt(label, '=');
        if (cur == NULL) { return NULL; }
        value = cur;
        cur = terminateAtOpts(value, "& ", &finish);
        if (cur == NULL) { return NULL; }
        int res = htAdd(headers->queryParameters, label, value);
        if (res != 0) { return NULL; }
        cur = cur + 1;
        // would be faster if made own compare function to add an equals to key if there isn't one
    }
    return cur;
}

char *manageVersion(requestHeaders *headers, const char *headersstr)
{
     if (! (strcmpequntil(&headersstr, "HTTP/1.", '.'))) {
         return NULL;
         // change to return 505 on HTTP2
     }
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
     if (headersstr[1] != '\r' || headersstr[2] != '\n') {
         return NULL;
         // return 400 bad request
     }
     
    return headersstr + 3;

    // && (strcmpequntil(&(headersstr[1]), "\r"))) // does &(headersstr[1]) have 
    // HTTP/1.0: no persistent connection
    // HTTP/1.1: keep connection by default

    // HTTP/1.1
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

// assuming provided headers string is byte after colon
char *manageHost(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    // I want to move to \r, but if there is whitespace, terminate at first whitespace
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->Host = headersstr;
    return cur + 1;
}
char *manageUA(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->UserAgent = headersstr;
    return cur + 1;
}

char *manageAccept(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->Accept = headersstr;
    return cur + 1;
}

char *manageAcceptLanguage(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->AcceptLanguage = headersstr;
    return cur + 1;
}

char *manageAcceptEncoding(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    char *cur = terminateAt(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->AcceptEncoding = headersstr;
    return cur + 1;
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
    }
    else {
        cur = headersstr;
        if (strcmpequntil(&cur, "close\r", '\r') == 1) {
            // strcmpequntil cannot possibly return 2 here because s2 has the delim
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

/*
* Possible returned status codes: 400, 501, 505
*/
char *parseHeaders(requestHeaders *headers, char *headersstr)
{
    char finish;
    if ((headersstr = extractMethod(headers, headersstr)) == NULL) { return STATUS501; }
    if ((headersstr = extractResource(headers, headersstr, &finish)) == NULL) { return STATUS400; }
    if (finish == '?') {
        if ((headersstr = extractQueryParameters(headers, headersstr)) == NULL) { return STATUS400; }
    }
    if ((headersstr = manageVersion(headers, headersstr)) == NULL) { return STATUS505; }
    // string literals are statically allocated according to C standard

    // exercise: linked list on stack
    // exercise hash table of function pointers

    while (headersstr[0] != 0 && headersstr[0] != '\r') {
        char* (*fn)(requestHeaders*, char*) = getHeaderFn(headersstr);
        if (fn == NULL) {
            headersstr = strchr(headersstr, '\r');
            if (headersstr == NULL || *(++headersstr) != '\n') { return STATUS400; }
            headersstr++;
        }
        else {
            headersstr = strchr(headersstr, ':') + 1;
            headersstr = fn(headers, headersstr);
            if (headersstr == NULL) { return STATUS400; }
        }
    }
    return NULL;


    // first line: replace first and second spaces with null byte
    // following lines: replace \r with null byte

    // resource:
    /*
    res = getFileName(requestbuf, &filename);
	if (res != 0) {
		fileNotFound(clientfd);
		// end this thread
		return 1;
	}
	
	res = sanitiseRequest(filename);
	if (res != 0) {
		fileNotFound(clientfd);
		//free(filename);
		return 1;
	}
	printf("Filename: %s\n", filename);
    */
}

/*
// request is null terminated within BUFSIZ
// edits request to null terminate filename
// and returns pointer to where filename starts
char *getResourceRequest(char *request) 
{
    // we are pointing to /
    if (request[0] != '/') {
        return NULL;
    }
    request++;
    // could do strlenuntil if want to go over string twice to avoid too much / too little memory allocated

	// we are pointing at char after /
	// could be null
	// turn first space into null:
    terminateAt(request, ' ');
    return request;
}
*/

void freeHeadersstr(char *headersstr) {
    free(headersstr);
}