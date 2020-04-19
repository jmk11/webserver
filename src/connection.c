#include <stdio.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <fcntl.h>
//#include <sys/types.h>
#include <string.h>
//#include <stdlib.h>
//#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
//#include <limits.h>
#include <linux/limits.h>
//#include <string.h>
#include <openssl/ssl.h>

#include "connection.h"
#include "wrappers/wrappers.h" // only needs this for error codes, this isn't right
#include "files/files.h"
#include "http/statuscodes.h"
#include "http/response.h"
#include "helpers/bool/bool.h"


#define FILESDIRECTORY "files/"

int handleRequest(SSL *ssl, char *requestbuf);//, ssize_t requestLength);
int getResource(const char *filename, char newResource[MAXPATH]);
//int setHTTPHeaders(responseHeaders headers, requestHeaders requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf);
char *getExtension(const char *filepath);
char *nullbyte(void);
char *buildFilePath(const char *filename);
//int fileNotFound(SSL *ssl);
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders) ;
//int fileNotAvailable(SSL *ssl);

int handleConnection(int clientfd, SSL_CTX *ctx) {
	char requestbuf[BUFSIZ];
	ssize_t requestLen;
	int retval = 0;

	// copied
	SSL *ssl = SSL_new(ctx);
	if (ssl == NULL) { 
		return 1;
	}
	if (SSL_set_fd(ssl, clientfd) == 0) {
		retval = 1;
	}
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
		bool continueConnection = FALSE;
		do {
			// set timeout on read
			requestLen = SSL_read(ssl, requestbuf, BUFSIZ-1);
			if (requestLen <= 0) {
				fprintf(stderr, "SSL_read error\n");
				continueConnection = FALSE;
			}
			else {
				requestbuf[requestLen] = 0;
				printf("%s\n", requestbuf);
				int res = handleRequest(ssl, requestbuf);
				continueConnection = (res >= 0);
				//retval = res;
			}
		} while (continueConnection);
	}
	SSL_shutdown(ssl); // surely this doesn't need to be called if accept() fails?
	SSL_free(ssl);
	return retval;
}

/*
* Return 0 on okay and continue connection
* Return 1 on bad and continue connection
* Return -1 on finish connection
*/
int handleRequest(SSL *ssl, char *requestbuf)//, ssize_t requestLength)
{
	printf("Response:\n");
	//int badReturn = 1;

	//char *filename = NULL;
	//char *headersbuf = NULL;
	//off_t filesize;
	off_t filesizePure;
	int filesize;
	//int headersLength;
	// the whole point of headersLength is eventually to be used as int parameter to ssl_write
	// but along the way i would like to be able to assume it is unsigned
	// so what to do?
	int res;
	char resource[MAXPATH];

	struct responseHeaders headers;
	initialiseResponseHeaders(&headers);

	struct requestHeaders requestHeaders;
	initialiseRequestHeaders(&requestHeaders);
	char *statusCode = parseHeaders(&requestHeaders, requestbuf);
	if (statusCode != NULL) {
		// return with status code
		setGenericHeaders(&headers, &requestHeaders);
		sendResponse(ssl, statusCode, &headers, requestHeaders.method, NULL, 0);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}
	// do not change requestbuf from this point on
	/*if (requestHeaders.method != METHOD_GET && requestHeaders.method != METHOD_HEAD) {
		// send method not supported or method not allowed
		setGenericHeaders(&headers, &requestheaders);
		sendResponse(ssl, STATUS_METHODNOTALLOWED, &headers, &requestHeaders, NULL, 0);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}*/
	
	res = getResource(requestHeaders.resource, resource);
	if (res != 0) {
		fileNotFound(ssl, &headers, &requestHeaders);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}
	printf("Filename: %s\n", resource);

	char *filepath = buildFilePath(resource);
	time_t lastModified;
	filesizePure = getFileDetails(filepath, &lastModified);
	if (filesizePure < 0) {
		switch (filesizePure) {
			case STAT_NOTREADABLE:
				fileNotFound(ssl, &headers, &requestHeaders);
				break;
			case STAT_NOTFOUND:
				fileNotFound(ssl, &headers, &requestHeaders);
				break;
			/*default:
				fileNotFound(ssl);
				free(filepath);
				return badReturn;*/
		}
		free(filepath);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}
	else if (filesizePure > INT_MAX) {
		res = fileNotFound(ssl, &headers, &requestHeaders);
		free(filepath);
		return requestHeaders.ConnectionKeep ? res : -1;
	}
	filesize = (int) filesizePure;

	if (lastModified <= requestHeaders.IfModifiedSince) {
		setGenericHeaders(&headers, &requestHeaders);
		res = sendResponse(ssl, STATUS_NOTMODIFIED, &headers, requestHeaders.method, NULL, 0);
		return requestHeaders.ConnectionKeep ? res : -1;
	}

	char *fileextension = getExtension(filepath);

	setGenericHeaders(&headers, &requestHeaders);
	setFileHeaders(&headers,&requestHeaders, lastModified, filesize, fileextension);
	//headersLength = setHTTPHeaders(responseHeaders, requestHeaders, lastModified, filesize, fileextension, &headersbuf);
	free(fileextension);

	res = sendResponse(ssl, STATUS_OK, &headers, requestHeaders.method, filepath, filesize);
	free(filepath);
	return requestHeaders.ConnectionKeep ? res : -1;
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

/*
* atm: returns 0 on success and 1 on failure
*/
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders) 
{
	int res;
	setGenericHeaders(headers, requestHeaders);
	char *filepath = buildFilePath("404.html");
	if (filepath == NULL) { return 1; }
	time_t modifiedSince;
	off_t filesizePure = getFileDetails(filepath, &modifiedSince);
	if (filesizePure < 0 || filesizePure > INT_MAX) {
		fprintf(stderr, "Could not access 404.html to send\n");
		res = sendResponse(ssl, STATUS_NOTFOUND, headers, requestHeaders->method, NULL, 0);
	}
	else {
		int filesize = (int) filesizePure;
		setFileHeaders(headers, requestHeaders, 0, filesize, "html");
		res = sendResponse(ssl, STATUS_NOTFOUND, headers, requestHeaders->method, filepath, filesize);
		free(filepath);
	}
	return res;

	/*
	unsigned int headersMax = 500;
	char *headersbuf = malloc(headersMax);
	//headersbuf[0] = 0;
	//char *headersbufcur = headersbuf;
	//strlcat3(headersbuf, &headersbufcur, "HTTP/1.1 404 Not Found\r\n\r\n", headersMax);
	//int headersLength = headersbufcur - headersbuf;
	int headersLength = snprintf(headersbuf, headersMax, "HTTP/1.1 404 Not Found\r\n\r\n");
	// I want to strcpy and return number of bytes copied
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
	*/
}

/*
int fileNotAvailable(SSL *ssl) 
{
	return 0;
}
*/
