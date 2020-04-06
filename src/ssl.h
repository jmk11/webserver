#include <openssl/ssl.h>

SSL_CTX *setupssl();
void cleanssl(SSL_CTX *ctx);
