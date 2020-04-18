#ifndef HEADERS_H
#define HEADERS_H

#include <stdlib.h>
#include <time.h>

//#include "constants.h"
#include "bool/bool.h"

struct headerPair {
    const char *label;
    char *value;
};
typedef struct headerPair headerPair;

// I think this strcut system was excessive, it's easy enough just to add a header manually using "Host: example.com"
// Could just have a function to append to headers string

struct responseHeaders {
    // general headers
    headerPair Date;
    headerPair Upgrade;
    // response headers
    headerPair Server;
    headerPair Location;
    headerPair RetryAfter;
    headerPair SetCookie;
    headerPair Tk; // ="N"
    headerPair XFrameOptions;
    headerPair XContentTypeOptions;
    headerPair ContentSecurityPolicy;
    // entity headers
    headerPair Allow;
    headerPair ContentLength;
    headerPair ContentLanguage;
    headerPair ContentLocation;
    headerPair ContentMD5;
    headerPair ContentType;
    headerPair LastModified;
};
typedef struct responseHeaders responseHeaders;

// structure to map value to statusCode

enum Method { 
    METHODINIT,
    METHOD_GET,
    METHOD_POST,
    METHOD_PUT,
    METHOD_PATH,
    METHOD_DELETE,
    METHOD_COPY,
    METHOD_HEAD,
    METHOD_OPTIONS,
    METHOD_LINK,
    METHOD_UNLINK,
    METHOD_PURGE,
    METHOD_LOCK,
    METHOD_UNLOCK,
    METHOD_PROPFIND,
    METHOD_VIEW
};

enum DNT {
    DNTINIT,
    DNT1,
    DNT0
};

enum UIR {
    UIRINIT,
    UIR1
};

// two INITS in different enums.. both at 0 but seems bad

struct headerPairBool {
    const char* const label;
    bool value;
};
typedef struct headerPairBool headerPairBool;

struct requestHeaders {
    enum Method method;
    char *resource;
    char *Host;
    char *UserAgent;
    char *Accept;
    char *AcceptLanguage;
    char *AcceptEncoding;
    enum DNT DNT;
    bool ConnectionKeep;
    enum UIR UpgradeInsecureRequests;
    char *Referer;
    time_t IfModifiedSince;
    //bool CKeepAlive;
};
typedef struct requestHeaders requestHeaders;

int initialiseResponseHeaders(responseHeaders *headers);
int initialiseRequestHeaders(requestHeaders *headers);
int parseHeaders(requestHeaders *headers, char *headersstr);
int produceHeaders(const char *statusCode, char **headersstrP, const responseHeaders *headers);


/*
struct headerValues {
    char *Allow;
    char *ContentLanguage;
    char *ContentLength;
    char *ContentLocation;
    char *ContentMD5;
    char *ContentType;
    char *Date;
    char *LastModified;
    char *Location;
    char *RetryAfter;
    char *Server;
    char *SetCookie;
    char *Tk; // ="N"
    char *Upgrade;
};
typedef struct headerValues headerValues;
*/

#endif /* HEADERS_H */
