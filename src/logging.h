#ifndef LOGGING_H
#define LOGGING_H

#include <netinet/in.h>

#include "http/headers/request/requestheaders.h"

int logRequest(int logfd, struct sockaddr_in addrStruct, const requestHeaders *requestHeaders);

#endif /* LOGGING_H */
