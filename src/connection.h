#ifndef CONNECTION_H
#define CONNECTION_H

#include <openssl/ssl.h>

#include "constants.h"

int handleConnection(int clientfd, SSL_CTX *ctx);

#endif /* CONNECTION_H */
