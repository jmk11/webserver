#ifndef SSL_H
#define SSL_H

#include <openssl/ssl.h>

SSL_CTX *setupssl(const char *configlocation);
void cleanssl(SSL_CTX *ctx);

#endif /* SSL_H */

