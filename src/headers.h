#ifndef HEADERS_H
#define HEADERS_H

#include <stdlib.h>

struct headerPair {
    const char *label;
    char *value;
};
typedef struct headerPair headerPair;

// I think this strcut system was excessive, it's easy enough just to add a header manually using "Host: example.com"
// Could just have a function to append to headers string

struct responseHeaders {
    headerPair Allow;
    headerPair ContentLength;
    headerPair ContentLanguage;
    headerPair ContentLocation;
    headerPair ContentMD5;
    headerPair ContentType;
    headerPair Date;
    headerPair LastModified;
    headerPair Location;
    headerPair RetryAfter;
    headerPair Server;
    headerPair SetCookie;
    headerPair Tk; // ="N"
    headerPair Upgrade;
    headerPair XFrameOptions;
    headerPair XContentTypeOptions;
    headerPair ContentSecurityPolicy;
};
typedef struct responseHeaders responseHeaders;

// structure to map value to statusCode

enum Method { 
    METHODINIT,
    GET,
    POST,
    PUT,
    PATH,
    DELETE,
    COPY,
    HEAD,
    OPTIONS,
    LINK,
    UNLINK,
    PURGE,
    LOCK,
    UNLOCK,
    PROPFIND,
    VIEW
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

struct requestHeaders {
    //char *method;
    enum Method method;
    char *resource;
    char *Host;
    char *UserAgent;
    char *Accept;
    char *AcceptLanguage;
    char *AcceptEncoding;
    //char *DNT;
    enum DNT DNT;
    char *Connection;
    //char *UpgradeInsecureRequests;
    enum UIR UpgradeInsecureRequests;
    char *Referer;
};
typedef struct requestHeaders requestHeaders;

int initialiseResponseHeaders(responseHeaders *headers);
int initialiseRequestHeaders(requestHeaders *headers);
int parseHeaders(requestHeaders *headers, char *headersstr);
int produceHeaders(char *statusCode, char **headersstr, const responseHeaders *headers);



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