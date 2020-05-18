#ifndef REQUESTHEADERS_H
#define REQUESTHEADERS_H

#include <time.h>

#include "../../helpers/bool/bool.h"
#include "../../helpers/hashtable/hashtableG.h"

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
    char *CacheControl;
    HashTable *queryParameters; // hashtable seems like overkill, normally only a few query parameters
    bool resourceMalloced; // Was the resource field malloced? If yes, free.
    // could do as array of structs {label, value}
};
typedef struct requestHeaders requestHeaders;

//int initialiseRequestHeaders(requestHeaders *headers);
const char *parseHeaders(requestHeaders *headers, char *headersstr);
void freeRequestHeaders(requestHeaders *headers);

char *manageHost(requestHeaders *headers, char *headersstr);
char *manageUA(requestHeaders *headers, char *headersstr);
char *manageAccept(requestHeaders *headers, char *headersstr);
char *manageAcceptLanguage(requestHeaders *headers, char *headersstr);
char *manageAcceptEncoding(requestHeaders *headers, char *headersstr);
char *manageDNT(requestHeaders *headers, char *headersstr);
char *manageConnection(requestHeaders *headers, char *headersstr);
char *manageUIR(requestHeaders *headers, char *headersstr);
char *manageReferer(requestHeaders *headers, char *headersstr);
char *manageIfModifiedSince(requestHeaders *headers, char *headersstr);
char *manageCacheControl(requestHeaders *headers, char *headersstr);

#endif /* REQUESTHEADERS_H */
