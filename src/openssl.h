#include <openssl/ssl.h>

void init_openssl();
void cleanup_openssl();
SSL_CTX *create_context();
void configure_context(SSL_CTX *ctx);
SSL_CTX *setupssl();
void cleanssl(SSL_CTX *ctx);
