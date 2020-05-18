/*
* Build responseHeaders struct into a headers string
*/

// #include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "headers.h"
#include "../../helpers/strings/strings.h"

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
    .ReferrerPolicy = {"Referrer-Policy", NULL},
    .ContentDisposition = {"Content-Disposition", NULL}
};

static const unsigned int numPairs = sizeof(struct responseHeaders) / sizeof(headerPair);

/*
 * Set up response headers struct - sets labels
 * Must be done before response headers struct is used
 * After calling this, you need only set .value on the headers you want and then call produceHeaders()
*/
int initialiseResponseHeaders(responseHeaders *headers) 
{
    *headers = responseHeadersBase;
    return 0;
}

/* 
 * Produce headers string from responseHeaders values.
 * Return headers length, or -1 if fail.
 * Sets *headersstrP to malloced memory.
*/
int produceHeaders(const char *status, char **headersstrP, const responseHeaders *headers)
{
    // TODO: check that doesn't pass headersmax. if so, realloc
    char *headersstr = *headersstrP;
    headersstr = malloc(HEADERSMAX);
    if (headersstr == NULL) {
        perror("Can't malloc headersstr");
        return -1;
    }
    //memset(*headersstr, 0, HEADERSMAX);
    headersstr[0] = 0;

    // Add status message
    char *headersstrcur = headersstr;
    const char* pieces[4] = {"HTTP/1.1 ", status, "\r\n", NULL};
    int res = strlcat4(headersstr, &headersstrcur, pieces, HEADERSMAX);
    if (res != 0) { free(headersstr); return -1; }
    /*int bytesWritten = snprintf(*headersstr, HEADERSMAX, "HTTP/1.1 %s OK\r\n", statusCode);
    char *headersstrcur = *headersstr + bytesWritten;*/

    // Add headers
    // I'm relying on struct layout which you aren't supposed to do but I want to try this
    const headerPair *pair = headers; // not casting because a warning seems fair
    for (unsigned int i = 0; i < numPairs; i++, pair++) {
        if (pair->value != NULL) {
            const char* pieces2[5] = {pair->label, ": ", pair->value, "\r\n", NULL};
            res = strlcat4(headersstr, &headersstrcur, pieces2, HEADERSMAX);
            if (res != 0) { free(headersstr); return -1; }
            // can do this with snprintf, but I would need to calculate the total number of bytes so I can check that it fits in
        }
    }

    // add final \r\n
    res = strlcat3(headersstr, &headersstrcur, "\r\n", HEADERSMAX);
    if (res != 0) { free(headersstr); return -1; }
    printf("\n%s\n", headersstr);    
    *headersstrP = headersstr;

    return (int) (headersstrcur - headersstr);
}
