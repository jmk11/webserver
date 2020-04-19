#include <string.h>
#include <stdio.h>

#include "responseheadersbase.h"
#include "../../../helpers/strings/strings.h"
//#include "headerfns.h"
#include "../../../helpers/hashtable/hash.h"
#include "../../statuscodes.h"

#define HEADERSMAX 1024

int initialiseResponseHeaders(responseHeaders *headers) 
{
    *headers = responseHeadersBase;
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
