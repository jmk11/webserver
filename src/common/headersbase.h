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
    .ContentSecurityPolicy =  {"Content-Security-Policy", NULL}
};

const struct requestHeaders requestHeadersBase = {
    //.method = NULL,
    .method = METHODINIT,
    .resource = NULL,
    .Host = NULL,
    .UserAgent = NULL,
    .Accept = NULL,
    .AcceptLanguage = NULL,
    .AcceptEncoding = NULL,
    //.DNT = NULL,
    .DNT = DNTINIT,
    .Connection = NULL,
    //.UpgradeInsecureRequests = NULL,
    .UpgradeInsecureRequests = UIRINIT,
    .Referer = NULL
};