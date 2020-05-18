#ifndef CONNECTION_H
#define CONNECTION_H

#include <openssl/ssl.h>
#include <netinet/in.h>

// #include "constants.h"

int handleConnection(int clientfd, SSL_CTX *ctx, int logfd, struct sockaddr_in source, const char *domain);

#endif /* CONNECTION_H */
