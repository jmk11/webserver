#ifndef REQUESTHEADERS_H
#define REQUESTHEADERS_H

#include <time.h>

#include "../../../helpers/bool/bool.h"
#include "../../../helpers/hashtable/hashtableG.h"

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
// could do struct of constants

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

struct queryParameter {
    const char *label;
    const char *value;
};

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
    HashTable *queryParameters;
};
typedef struct requestHeaders requestHeaders;

int initialiseRequestHeaders(requestHeaders *headers);
char *parseHeaders(requestHeaders *headers, char *headersstr);
void freeRequestHeaders(requestHeaders *headers);

#endif /* REQUESTHEADERS_H */
