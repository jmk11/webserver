#include <openssl/err.h>
#include <fcntl.h>
#include <unistd.h>

#include "ssl.h"
#include "helpers.h"

#define PATHLENGTH 150
#define CONFIGLOCATION "config/certs.txt"

void init_openssl();
SSL_CTX *create_context();
int configure_context(SSL_CTX *ctx);
void cleanup_openssl();
int readCertLocation(char certLocation[PATHLENGTH], char keyLocation[PATHLENGTH]);

SSL_CTX *setupssl()
{
    init_openssl();
	SSL_CTX *ctx = create_context();
	int res = configure_context(ctx);
    if (res != 0) {
        fprintf(stderr, "Unable to configure SSL.\n");
        SSL_CTX_free(ctx);
        return NULL;
    }
    return ctx;
}

void cleanssl(SSL_CTX *ctx)
{
    SSL_CTX_free(ctx);
	cleanup_openssl();
}

// these functions copied from 
// https://wiki.openssl.org/index.php/Simple_TLS_Server

void init_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

int configure_context(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    char certLocation[PATHLENGTH];
    char keyLocation[PATHLENGTH];
    int res = readCertLocation(certLocation, keyLocation);
    if (res != 0) {
        return 1;
    }

    /* Set the key and cert */
    //if (SSL_CTX_use_certificate_file(ctx, certLocation, SSL_FILETYPE_PEM) <= 0) {
    if (SSL_CTX_use_certificate_chain_file(ctx, certLocation) <= 0) {
        ERR_print_errors_fp(stderr);
	    return 1;
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, keyLocation, SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	    return 1;
    }

    if (SSL_CTX_check_private_key(ctx) != 1) {
        // I'm not quite sure how this function works
        return 1;
    }

    return 0;
}

// end copying

int readCertLocation(char certLocation[PATHLENGTH], char keyLocation[PATHLENGTH])
{
    int fd = open(CONFIGLOCATION, O_RDONLY);
    if (fd < 0) {
        perror("Can't open config file");
        return 1;
    }
    // obviously deal with size etc.
    char buf[500];
    int bytesRead = read(fd, buf, 500-1);
    if (bytesRead <= 0) {
        perror("Can't read config file");
        return 1;
    }
    close(fd);
    buf[bytesRead] = 0;

    // get cert location
    char *cur;
    int res = strncpyuntil(certLocation, buf, PATHLENGTH, '\n', &cur);
    if (res != 0) {
        fprintf(stderr, "Certicate location invalid. It may be nonexistent or too long. Max length %d.\n", PATHLENGTH-1);
        return 1;
    }
    // get privkey location
    res = strncpyuntil(keyLocation, cur, PATHLENGTH, '\n', NULL);
    if (res != 0) {
        fprintf(stderr, "Key location invalid. It may be nonexistent or too long. Max length %d.\n", PATHLENGTH-1);
        return 1;
    }
    
    return 0;
}
