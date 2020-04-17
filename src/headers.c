#include <string.h>
#include <stdio.h>

#include "headers.h"
#include "helpers.h"
#include "headersbase.h"
#include "strings/strings.h"
//#include "statusmessage.h"

#define HEADERSMAX 1024

char *getResourceRequest(char *request);


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

int parseHeaders(requestHeaders *headers, char *headersstr)
{
    // method:
    if (strcmpequntil(&headersstr, "GET", ' ') == 1) {
        headers->method = METHOD_GET;
    }
    else if (strcmpequntil(&headersstr, "HEAD", ' ') == 1) {
        headers->method = METHOD_HEAD;
    }
    else {
        return 1;
    }
    // could do with hashtable after replacing first space with null byte
    // resource:
    if (headersstr[0] != '/') {
        return 1;
    }
    headers->resource = headersstr+1;
    headersstr = terminateAt(headers->resource, ' ') + 1;

    // HTTP/1.1
    if (! (
        (strcmpequntil(&headersstr, "HTTP/1", '.'))
        && (headersstr[0] == '0' || headersstr[0] == '1')
        && (headersstr[1] == '\r' && headersstr[2] == '\n')
        )) {
        return 1;
    }
    // && (strcmpequntil(&(headersstr[1]), "\r"))) // does &(headersstr[1]) have 

    headersstr = headersstr + 3;

    // exercise: linked list on stack
    




    
    

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
    return 0;
}

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

void freeHeadersstr(char *headersstr) {
    free(headersstr);
}