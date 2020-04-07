#include <stdio.h>
//#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
//#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>

#include "helpers.h"
#include "constants.h"
#include "uid.h"
#include "headers.h"
#include "custom.h"
#include "ssl.h"
#include "common/common.h"
#include "contenttype.h"

#define LOGFILE "logs/serverlog.txt"
bool fileLogging = TRUE;


int handleRequest(SSL *ssl, char *requestbuf);//, ssize_t requestLength);

int getFileName(char *request, char **filename);
int getResource(const char *filename, char newResource[MAXPATH]);
int loadRequestedFile(const char *filename, char **filebuf, off_t *fileLength);
int loadHTTPHeaders(off_t fileLength, char *filetype, char **headersbuf, unsigned long *headersLength);

char *getExtension(const char *filename);
int handleConnection(int clientfd, SSL_CTX *ctx);

int fileNotFound(SSL *ssl);
int fileNotAvailable(SSL *ssl);

int main(int argc, char **argv)
{
	SSL_CTX *ctx = setupssl();
	if (ctx == NULL) {
		fprintf(stderr, "SSL setup failure. Exiting.\n");
		return 1;
	}

	printf("uid at start: %d\n", getuid());
	
	unsigned short port;
	if (argc == 2) {
		port = getPort(argv[1]);
	} 
	else {
		port = 443;
	}
	printf("Using port number %hu\n", port);
	
	int serverfd = buildSocket(port);
	
	// after binding port 80, can drop permissions
	dropPermissions(UID);

	//int res;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientfd;
	//char reqFileName[MAXPATH];
	//char *filebuf;

	int logfd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0660);
	if (logfd < 0) {
		perror("Cannot open log file for writing");
		fprintf(stderr, "Continuing execution without logging to file..");
		fileLogging = FALSE;
	}

	int res = buildContentTypeHT();
	if (res != 0) {
		fprintf(stderr, "Couldn't build content type hash table.\nExiting.\n");
		return 1;
	}
	
	printf("Entering accept loop..\n");
	while (1) {
		clientfd = accept(serverfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		if (clientfd < 0) {
			perror("accept clientfd < 0");
		}
		else {
			printSource(clientAddr, NULL);
			if (fileLogging) { logSource(logfd, clientAddr); }
			handleConnection(clientfd, ctx);
			close(clientfd);
			// what do if can't close client socket?
		}
	}
	
	Close(logfd);
	Close(serverfd);
	cleanssl(ctx);
	return 0;	
}

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
		//free(filename);
		return 1;
	}
	printf("Filename: %s\n", resource);
	
	res = loadRequestedFile(resource, &filebuf, &fileLength);
	if (res != 0) {
		fileNotFound(ssl);
		//free(filename);
		return 1;
	}
	// won't scale with large file size
	//free(filename);
	char *fileextension = getExtension(resource);
	
	res = loadHTTPHeaders(fileLength, fileextension, &headersbuf, &headersLength);
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

int loadRequestedFile(const char *filename, char **filebuf, off_t *fileLength) 
{
	char *prefix = "files/";
	char *filepath = malloc(strlen(filename) + strlen(prefix) + 1);
	if (filepath == NULL) {
		return MALLOCERROR;
	}
	strcpy(filepath, prefix);
	strcat(filepath, filename);
	printf("Filepath: %s\n", filepath);
	
	struct stat filestat;
	int res = stat(filepath, &filestat);
	if (res != 0) {
		free(filepath);
		return FILEERROR;
	}
	*fileLength = filestat.st_size;
	// stat has time of last modification, for cache
	// if file permissions suit
	
	int fd = open(filepath, O_RDONLY);
	if (fd < 0) {
		free(filepath);
		return FILEERROR;
	}
	*filebuf = malloc(*fileLength);
	if (*filebuf == NULL) {
		free(filepath);
		return MALLOCERROR;
	}
	ssize_t bytesRead = read(fd, *filebuf, *fileLength);
	if (bytesRead != *fileLength) {
		free(filepath);
		return FILEERROR;
	}
	
	res = close(fd);
	if (res != 0) {
		perror("Error close");
	}
	free(filepath);
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

char *getExtension(const char *filename) {
	char *lastdot = NULL;
	for (; *filename != 0; filename++) {
		if (*filename == '.') {
			lastdot = filename;
		}
	}
	return lastdot+1;
	// what if it is a symbolic link?
	// have to get extension of linking file
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
