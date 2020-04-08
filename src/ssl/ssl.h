#include <openssl/ssl.h>

SSL_CTX *setupssl(const char *configlocation);
void cleanssl(SSL_CTX *ctx);
