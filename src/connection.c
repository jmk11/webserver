#include <stdio.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
//#include <limits.h>
#include <linux/limits.h>
//#include <string.h>
#include <openssl/ssl.h>

#include "connection.h"
#include "helpers.h"
//#include "constants.h"
#include "headers.h"
#include "custom.h"
//#include "common.h"
#include "contenttype.h"
#include "wrappers.h" // only needs this for error codes, this isn't right

#define FILESDIRECTORY "files/"

int handleRequest(SSL *ssl, char *requestbuf);//, ssize_t requestLength);
char *buildFilePath(const char *filename);
int getResource(const char *filename, char newResource[MAXPATH]) ;
int loadRequestedFile(const char *filepath, char **filebuf, off_t *fileLength) ;
int loadHTTPHeaders(off_t fileLength, char *fileExtension, char **headersbuf, unsigned long *headersLength) ;
char *getExtension(const char *filepath);
char *nullbyte();
int fileNotFound(SSL *ssl);
int fileNotAvailable(SSL *ssl);

int handleConnection(int clientfd, SSL_CTX *ctx) {
	char requestbuf[BUFSIZ];
	ssize_t requestLen;
	int retval = 0;

	// copied
	SSL *ssl = SSL_new(ctx);
	SSL_set_fd(ssl, clientfd);
	if (SSL_accept(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
		retval = 1;
	}
	// end copied
	/*SSL_set_accept_state(ssl);
	if (SSL_do_handshake(ssl) <= 0) {
		ERR_print_errors_fp(stderr);
	}*/
	else {
		/*requestLen = recv(clientfd, requestbuf, BUFSIZ-1, 0);
		if (requestLen < 0) {
			perror("Error recv");
		}*/
		requestLen = SSL_read(ssl, requestbuf, BUFSIZ-1);
		if (requestLen <= 0) {
			fprintf(stderr, "SSL_read error\n");
		}
		else {
			requestbuf[requestLen] = 0;
			printf("%s\n", requestbuf);
			int res = handleRequest(ssl, requestbuf);
			retval = res;
		}
	}
	SSL_shutdown(ssl);
	SSL_free(ssl);
	return retval;
}

int handleRequest(SSL *ssl, char *requestbuf)//, ssize_t requestLength)
{
	printf("Response:\n");

	char *filename = NULL;
	char *filebuf = NULL;
	char *headersbuf = NULL;
	off_t fileLength = 0;
	unsigned long headersLength = 0;
	int res;
	char resource[MAXPATH];

	struct requestHeaders requestHeaders;
	initialiseRequestHeaders(&requestHeaders);
	parseHeaders(&requestHeaders, requestbuf);
	
	res = getResourceRequest(requestbuf, &filename);
	if (res != 0) {
		fileNotFound(ssl);
		// end this thread
		return 1;
	}
	
	res = getResource(filename, resource);
	if (res != 0) {
		fileNotFound(ssl);
		return 1;
	}
	printf("Filename: %s\n", resource);

	char *filepath = buildFilePath(resource);
	res = loadRequestedFile(filepath, &filebuf, &fileLength);
	if (res != 0) {
		fileNotFound(ssl);
		free(filepath);
		return 1;
	}
	// won't scale with large file size
	char *fileextension = getExtension(filepath);
	free(filepath);
	
	res = loadHTTPHeaders(fileLength, fileextension, &headersbuf, &headersLength);
	free(fileextension);
	//res = send(clientfd, headersbuf, headersLength, 0);
	res = SSL_write(ssl, headersbuf, headersLength);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send HTTP headers");
		free(filebuf);
		return 1;
	}
	
	//res = send(clientfd, filebuf, fileLength, 0);
	res = SSL_write(ssl, filebuf, fileLength);
	if (res != fileLength) {
		perror("Error send file");
		free(filebuf);
		return 1;
	}
	free(filebuf);
	return 0;
}

char *buildFilePath(const char *filename)
{
	char *prefix = FILESDIRECTORY;
	char *filepath = malloc(strlen(filename) + strlen(prefix) + 1);
	if (filepath == NULL) {
		perror("Can't malloc filepath");
		return NULL;
	}
	strcpy(filepath, prefix);
	strcat(filepath, filename);
	printf("Filepath: %s\n", filepath);
	return filepath;
}

// filename length < bufsiz
int getResource(const char *filename, char newResource[MAXPATH]) 
{
	for (unsigned int i = 0; filename[i+1] != 0; i++) {
		if (filename[i] == '.' && filename[i+1] == '.') {
			return 1;
		}
	}
	// I don't like this way
	if (filename[0] == 0) {
		strncpy(newResource, "index.html", MAXPATH); // check this isn't 1 byte overflow
	}
	else {
		strncpy(newResource, filename, MAXPATH);
	}
	return 0;
}

// sets filebuf pointer to malloced memory that must be freed
int loadRequestedFile(const char *filepath, char **filebuf, off_t *fileLength) 
{	
	struct stat filestat;
	int res = stat(filepath, &filestat);
	if (res != 0) {
		return FILEERROR;
	}
	*fileLength = filestat.st_size;
	// stat has time of last modification, for cache
	// if file permissions suit
	
	int fd = open(filepath, O_RDONLY);
	if (fd < 0) {
		return FILEERROR;
	}
	*filebuf = malloc(*fileLength);
	if (*filebuf == NULL) {
		return MALLOCERROR;
	}
	ssize_t bytesRead = read(fd, *filebuf, *fileLength);
	if (bytesRead != *fileLength) {
		return FILEERROR;
	}
	
	res = close(fd);
	if (res != 0) {
		perror("Error close");
	}
	return 0;
}

#define OFF_TDIGITS 19
#define OFF_TSTRMAX (OFF_TDIGITS+1)


int loadHTTPHeaders(off_t fileLength, char *fileExtension, char **headersbuf, unsigned long *headersLength) 
{
	responseHeaders headers;
	initialiseResponseHeaders(&headers);

	// maximum signed 64 bit number 9,223,372,036,854,775,807 -> 19 digits
	char contentLength[OFF_TSTRMAX];
	snprintf(contentLength, OFF_TSTRMAX, "%ld", fileLength);
	headers.ContentLength.value = contentLength;

	char datebuf[MAXLENGTH];
	int res = getFormattedDate(datebuf, MAXLENGTH);
	if (res == 0) {
		headers.Date.value = datebuf;
	}
	// this is a bad way of doing it as all the headers must be in ram in different strings at the same time

	addMyHeaders(&headers);

	setContentType(fileExtension, &(headers.ContentType.value));
	
	res = produceHeaders("200", headersbuf, &headers);
	if (res != 0) {
		free(*headersbuf);
		return 1;
	}
	*headersLength = strlen(*headersbuf);

	/*
	unsigned int headersMax = 500;
	*headersbuf = malloc(headersMax);
	char *headersbufcur = *headersbuf;
	strlcat3(&headersbufcur, "HTTP/1.1 200 OK\r\n", headersMax);
	char contentLength[100];
	snprintf(contentLength, 100, "Content-Length: %lu\r\n", fileLength);
	strlcat3(&headersbufcur, contentLength, headersMax);
	strlcat3(&headersbufcur, "\r\n", headersMax);
	*headersLength = headersbufcur - *headersbuf;
	*/
	return 0;
}

// probably worth moving this to some other place where stat() is already called
// returns malloced string that must be freed
char *getExtension(const char *filepath) {
	struct stat filestat;
	int res = lstat(filepath, &filestat);
	if (res != 0) {
		perror("lstat on filename failed");
		return nullbyte();
	}
	
	char targetpath[PATH_MAX];
	mode_t fileType = filestat.st_mode & S_IFMT;
	if (fileType == S_IFLNK) {
		// symbolic link
		char *res = realpath(filepath, targetpath);
		if (res == NULL) {
			perror("realpath failed");
			return nullbyte();
		}
		else {
			filepath = targetpath;
		}
	}

	char *lastdot = NULL;
	for (; *filepath != 0; filepath++) {
		if (*filepath == '.') {
			lastdot = filepath;
		}
	}
	if (lastdot == NULL) { 
		return nullbyte(); 
	}
	return strdup(lastdot+1);

	// what if it is a symbolic link?
	// have to get extension of linking file
}

char *nullbyte() {
	char *byte = malloc(1);
	byte[0] = 0;
	return byte;
}

int fileNotFound(SSL *ssl) 
{
	unsigned int headersMax = 500;
	char *headersbuf = malloc(headersMax);
	headersbuf[0] = 0;
	char *headersbufcur = headersbuf;
	strlcat3(headersbuf, &headersbufcur, "HTTP/1.1 404 Not Found\r\n\r\n", headersMax);
	unsigned int headersLength = headersbufcur - headersbuf;
	//int res = send(clientfd, headersbuf, headersLength, 0);
	int res = SSL_write(ssl, headersbuf, headersLength);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send 404 header");
		return 1;
	}

	
	char *response = "404";
	//res = send(clientfd, response, strlen(response), 0);
	res = SSL_write(ssl, response, strlen(response));
	if (res != strlen(response)) {
		perror("Error send file");
		return 1;
	}
	return 0;
}
int fileNotAvailable(SSL *ssl) 
{
	return 0;
}