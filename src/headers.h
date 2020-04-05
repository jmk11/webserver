#ifndef HEADERS_H
#define HEADERS_H

#include <stdlib.h>

struct headerPair {
    const char *label;
    char *value;
};
typedef struct headerPair headerPair;

struct headers {
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
};
typedef struct headers headers;

// structure to map value to statusCode


int initialiseHeaders(headers *headers);
int produceHeaders(char *statusCode, char **headersstr, const headers *headers);

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