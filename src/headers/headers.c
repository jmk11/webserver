#include <string.h>
#include <stdio.h>

#include "helpers/helpers.h"
#include "headersbase.h"
#include "strings/strings.h"
//#include "headerpriv.h"
//#include "hashtable/hashtableG.h"
//#include "statusmessage.h"
#include "headerfns.h"
//#include "headers.h"

#define HEADERSMAX 1024

char *extractMethod(requestHeaders *headers, const char *headersstr);
char *extractResource(requestHeaders *headers, char *headersstr);
char *manageVersion(requestHeaders *headers, const char *headersstr);


int initialiseResponseHeaders(responseHeaders *headers) 
{
    *headers = responseHeadersBase;
    return 0;
}

int initialiseRequestHeaders(requestHeaders *headers) 
{
    *headers = requestHeadersBase;
    return 0;
}

/*
int addHeader(const char *header, char *headersstr, unsigned int headersstrlength)
{
    char *headersstrcur = headersstr;
    int res = strlcat3(headersstr, &headersstrcur, header, headersstrlength);
    if (res != 0) { return 1; }
    int res = strlcat3(headersstr, &headersstrcur, "\r\n", headersstrlength);
    if (res != 0) { return 1; }
    return 0;
    // could just do an snprintf on headersstrcur with remaining length
}*/

// produce headers string from responseHeaders values
// return headers length, or -1 if fail
int produceHeaders(const char *status, char **headersstrP, const responseHeaders *headers)
{
    // check that doesn't pass headersmax. if so, realloc
    char *headersstr = *headersstrP;
    headersstr = malloc(HEADERSMAX);
    if (headersstr == NULL) {
        perror("Can't malloc headersstr");
        return -1;
    }
    //memset(*headersstr, 0, HEADERSMAX);
    headersstr[0] = 0;
    char *headersstrcur = headersstr;
    /*
    int res = strlcat3(*headersstr, &headersstrcur, "HTTP/1.1 ", HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, statusCode, HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, " ", HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, getStatusMessage(statusCode), HEADERSMAX); // obv change this
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, "\r\n", HEADERSMAX); // obv change this
    if (res != 0) { free(*headersstr); return 1; }
    */
    // const char* pieces[6] = {"HTTP/1.1 ", statusCode, " ", getStatusMessage(statusCode), "\r\n", NULL};
    const char* pieces[4] = {"HTTP/1.1 ", status, "\r\n", NULL};
    int res = strlcat4(headersstr, &headersstrcur, pieces, HEADERSMAX);
    if (res != 0) { free(headersstr); return -1; }
    /*int bytesWritten = snprintf(*headersstr, HEADERSMAX, "HTTP/1.1 %s OK\r\n", statusCode);
    char *headersstrcur = *headersstr + bytesWritten;*/

    // I'm relying on struct layout which you aren't supposed to do but I want to try this
    const headerPair *pair = headers;
    unsigned int numPairs = sizeof(struct responseHeaders) / sizeof(headerPair);
    for (unsigned int i = 0; i < numPairs; i++, pair++) {
        if (pair->value != NULL) {
            const char* pieces2[5] = {pair->label, ": ", pair->value, "\r\n", NULL};
            res = strlcat4(headersstr, &headersstrcur, pieces2, HEADERSMAX);
            if (res != 0) { free(headersstr); return -1; }
            /*
            res = strlcat3(*headersstr, &headersstrcur, pair->label, HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, ": ", HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, pair->value, HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, "\r\n", HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            */
            // can do this with one snprintf, but I would need to calculate the total number of bytes
            // I intend to write so I can know if it is less
        }
    }
    res = strlcat3(headersstr, &headersstrcur, "\r\n", HEADERSMAX);
    if (res != 0) { free(headersstr); return -1; }
    printf("\n%s\n", headersstr);    
    *headersstrP = headersstr;

    return headersstrcur - headersstr;
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
char *extractResource(requestHeaders *headers, char *headersstr)
{
    if (headersstr[0] != '/') {
        return NULL;
    }
    headers->resource = headersstr+1;
    char *cur = terminateAt(headers->resource, ' ');
    if (cur == NULL) { return NULL; }
    return cur + 1;
}

char *manageVersion(requestHeaders *headers, const char *headersstr)
{
     if (! (strcmpequntil(&headersstr, "HTTP/1.", '.'))) {
         return NULL;
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
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    headers->AcceptLanguage = headersstr;
    return cur + 1;
    // store if same domain or do elsewhere?
}

char *manageIfModifiedSince(requestHeaders *headers, char *headersstr)
{
    headersstr = skipwsp(headersstr);
    time_t mstime = HTTPDatetotime(headersstr);
    if (mstime < 0) { return NULL; }
    headers->IfModifiedSince = mstime;
    char *cur = strchr(headersstr, '\r');
    if (cur == NULL || *(++cur) != '\n') { return NULL; }
    return cur + 1;
}

int parseHeaders(requestHeaders *headers, char *headersstr)
{
    // method:
    if ((headersstr = extractMethod(headers, headersstr)) == NULL) { return 501; }
    if ((headersstr = extractResource(headers, headersstr)) == NULL) { return 400; }
    if ((headersstr = manageVersion(headers, headersstr)) == NULL) { return 505; }

    // exercise: linked list on stack
    // exercise hash table of function pointers

    while (headersstr[0] != 0 && headersstr[0] != '\r') {
        char* (*fn)(requestHeaders*, char*) = getHeaderFn(headersstr);
        if (fn == NULL) {
            headersstr = strchr(headersstr, '\r');
            if (headersstr == NULL || *(++headersstr) != '\n') { return 400; }
            headersstr++;
        }
        else {
            headersstr = strchr(headersstr, ':') + 1;
            headersstr = fn(headers, headersstr);
            if (headersstr == NULL) { return 400; }
        }
    }
    return 0;


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