#ifndef HEADERS_H
#define HEADERS_H

#include <stdlib.h>

struct headerPair {
    const char *label;
    char *value;
};
typedef struct headerPair headerPair;

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
    INIT,
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

struct requestHeaders {
    //char *method;
    enum Method method;
    char *resource;
    char *Host;
    char *UserAgent;
    char *Accept;
    char *AcceptLanguage;
    char *AcceptEncoding;
    char *DNT;
    char *Connection;
    char *UpgradeInsecureRequests;
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