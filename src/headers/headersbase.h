#include <stdlib.h>

#include "headers.h"

const struct responseHeaders responseHeadersBase = {
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
    .LastModified = {"Last-Modified", NULL}
};

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
    .IfModifiedSince = -1
};
