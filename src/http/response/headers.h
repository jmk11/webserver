#ifndef HEADERS_H
#define HEADERS_H

struct headerPair {
    const char *label; // const char* const label
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
    headerPair StrictTransportSecurity;
    headerPair ReferrerPolicy;
    // entity headers
    headerPair Allow;
    headerPair ContentDisposition;
    headerPair ContentLength;
    headerPair ContentLanguage;
    headerPair ContentLocation;
    headerPair ContentMD5;
    headerPair ContentType;
    headerPair LastModified;
};
typedef struct responseHeaders responseHeaders;

int initialiseResponseHeaders(responseHeaders *headers);
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
