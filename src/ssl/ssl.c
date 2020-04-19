#include <openssl/err.h>
#include <fcntl.h>
#include <unistd.h>

#include "ssl.h"
#include "../helpers/strings/strings.h"

#define PATHLENGTH 150

void init_openssl(void);
void cleanup_openssl(void);
SSL_CTX *create_context(void);
int configure_context(SSL_CTX *ctx, const char *configlocation);
int readCertLocation(const char *configlocation, char certLocation[PATHLENGTH], char keyLocation[PATHLENGTH]);

SSL_CTX *setupssl(const char *configlocation)
{
    init_openssl();
	SSL_CTX *ctx = create_context();
    if (ctx != NULL) {
        int res = configure_context(ctx, configlocation);
        if (res != 0) {
            fprintf(stderr, "Unable to configure SSL.\n");
            SSL_CTX_free(ctx);
            return NULL;
        }
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
    EVP_cleanup(); // wth this does nothing?
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
        return NULL;
        //exit(EXIT_FAILURE);
    }

    return ctx;
}

int configure_context(SSL_CTX *ctx, const char *configlocation)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    char certLocation[PATHLENGTH];
    char keyLocation[PATHLENGTH];
    int res = readCertLocation(configlocation, certLocation, keyLocation);
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

int readCertLocation(const char *configlocation, char certLocation[PATHLENGTH], char keyLocation[PATHLENGTH])
{
    int fd = open(configlocation, O_RDONLY);
    if (fd < 0) {
        perror("Can't open config file");
        return 1;
    }
    // obviously deal with size etc.
    char buf[500];
    ssize_t bytesRead = read(fd, buf, 500-1);
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

// With this function here, this file is independent and can be copied as a library
/*
int strncpyuntil(char *dest, const char *src, unsigned int length, char delim, char **cur)
{
    unsigned int i = 0;
    while (src[i] != 0 && src[i] != delim && i < length-1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    if (src[i] != delim) {
        return 1;
    }
    if (cur != NULL) {
        if (src[i] == 0) {
            *cur = &(src[i]);
        }
        else {
            *cur = &(src[i+1]);
        }
    }

    return 0;
}
*/
