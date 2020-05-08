#ifndef RESPONSE_H
#define RESPONSE_H

#include <openssl/ssl.h>

#include "headers.h"
#include "../request/requestheaders.h"

//int sendResponse(SSL *ssl, const char *headersbuf, int headersLength, const char *filepath, size_t filesize);
//void setGenericHeaders(responseHeaders *headers, const requestHeaders *requestHeaders);
//void setFileHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension);
//int buildHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf);
//int buildHeadersNoBody(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, char **headersbuf);

int sendResponse(SSL *ssl, const char *statusCode, responseHeaders *headers, const requestHeaders *requestHeaders, const char *body, int bodylength, time_t lastModified, const char *fileextension);
int sendResponseBuffered(SSL *ssl, const char *statusCode, responseHeaders *headers, const requestHeaders *requestHeaders, int fd, int filesize, time_t lastModified, const char *fileextension);
int sendResponseNoBody(SSL *ssl, responseHeaders *headers, const char *statusCode, const requestHeaders *requestHeaders);

#endif /* RESPONSE_H */
