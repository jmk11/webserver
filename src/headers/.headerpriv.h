#ifndef HEADERFNS_H
#define HEADERFNS_H

#include "headers.h"

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


#endif /* HEADERFNS_H */
