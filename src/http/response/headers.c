#include <string.h>
#include <stdio.h>

#include "headers.h"
//#include "responseheadersbase.h"
#include "../../helpers/strings/strings.h"
//#include "headerfns.h"
#include "../../helpers/hashtable/hash.h"
#include "../statuscodes.h"

#define HEADERSMAX 1024

// why don't I include the colon?
static const struct responseHeaders responseHeadersBase = {
    .Allow = {"Allow", NULL}, 
    .ContentLength = {"Content-Length", NULL},
    .ContentLanguage = {"Content-Language", NULL},
    .ContentLocation = {"Content-Location", NULL},
    .ContentMD5 = {"Content-MD5", NULL},
    .ContentType = {"Content-Type", NULL},
    .Date = {"Date", NULL},
    .LastModified = {"Last-Modified", NULL},
    .Location = {"Location", NULL},
    .RetryAfter = {"Retry-After", NULL},
    .Server = {"Server", NULL},
    .SetCookie = {"Set-Cookie", NULL},
    .Tk = {"Tk", NULL}, // ="N"
    .Upgrade = {"Upgrade", NULL},
    .XFrameOptions = {"X-Frame-Options", NULL},
    .XContentTypeOptions = {"X-Content-Type-Options", NULL},
    .ContentSecurityPolicy =  {"Content-Security-Policy", NULL},
    .StrictTransportSecurity = {"Strict-Transport-Security", NULL},
    //.StrictTransportSecurity = {"Strict-Transport-Security: ", NULL}
    .ReferrerPolicy = {"Referrer-Policy", NULL}
};

int initialiseResponseHeaders(responseHeaders *headers) 
{
    *headers = responseHeadersBase;
    return 0;
}

// produce headers string from responseHeaders values
// return headers length, or -1 if fail
// sets *headersstrP to malloced memory
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
