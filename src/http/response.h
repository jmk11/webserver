#ifndef RESPONSE_H
#define RESPONSE_H

#include <openssl/ssl.h>

#include "headers/response/headers.h"
#include "headers/request/requestheaders.h"

int sendResponse(SSL *ssl, const char *statusCode, const responseHeaders *headers, enum Method method, const char *filepath, size_t filesize);
void setGenericHeaders(responseHeaders *headers, const requestHeaders *requestHeaders);
void setFileHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension) ;

#endif /* RESPONSE_H */
