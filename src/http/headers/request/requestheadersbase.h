#ifndef REQUESTHEADERSBASE_H
#define REQUESTHEADERSBASE_H

#include "requestheaders.h"

/*
const struct requestHeaders requestHeadersBase = {
    .method = METHODINIT,
    .resource = NULL,
    .Host = {"Host", NULL},
    .UserAgent = {"User-Agent", NULL},
    .Accept = {"Accept", NULL},
    .AcceptLanguage = {"Accept-Language", NULL},
    .AcceptEncoding = {"Accept-Encoding", NULL},
    .DNT = {"DNT", TRUE},
    .ConnectionKeep = {"Connection", TRUE},
    .UpgradeInsecureRequests = {"Upgrade-Insecure-Requests", FALSE},
    .Referer = {"Referer", NULL}
};
*/


const struct requestHeaders requestHeadersBase = {
    .method = METHODINIT,
    .resource = NULL,
    .Host = NULL,
    .UserAgent = NULL,
    .Accept = NULL,
    .AcceptLanguage = NULL,
    .AcceptEncoding = NULL,
    .DNT = DNTINIT,
    .ConnectionKeep = TRUE,
    .UpgradeInsecureRequests = UIRINIT,
    .Referer = NULL,
    .IfModifiedSince = -1,
    .queryParameters = NULL
};

#endif /* REQUESTHEADERSBASE_H */

