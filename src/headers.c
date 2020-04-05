#include <string.h>

#include "headers.h"
#include "helpers.h"
#include "headersbase.h"

#define HEADERSMAX 1024


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

int produceHeaders(char *statusCode, char **headersstr, const responseHeaders *headers)
{
    // check that doesn't pass headersmax. if so, realloc
    *headersstr = malloc(HEADERSMAX);
    if (*headersstr == NULL) {
        perror("Can't malloc headersstr");
        return 1;
    }
    memset(*headersstr, 0, HEADERSMAX);
    char *headersstrcur = *headersstr;
    int res = strlcat3(*headersstr, &headersstrcur, "HTTP/1.1 ", HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, statusCode, HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, " OK\r\n", HEADERSMAX); // obv change this
    if (res != 0) { free(*headersstr); return 1; }

    // I'm relying on struct layout which you aren't supposed to do but I want to try this
    headerPair *pair = headers;
    unsigned int numPairs = sizeof(struct responseHeaders) / sizeof(headerPair);
    for (unsigned int i = 0; i < numPairs; i++, pair++) {
        if (pair->value != NULL) {
            res = strlcat3(*headersstr, &headersstrcur, pair->label, HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, ": ", HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, pair->value, HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, "\r\n", HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
        }
    }
    res = strlcat3(*headersstr, &headersstrcur, "\r\n", HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    printf("headers: %s\n", *headersstr);    

    return 0;
}

int parseHeaders(requestHeaders *headers, char *headersstr)
{
    return 0;
}

void freeHeadersstr(char *headersstr) {
    free(headersstr);
}