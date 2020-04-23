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
#include "logging.h"
#include "wrappers/wrappers.h" // only needs this for error codes, this isn't right
#include "files/files.h"
#include "http/statuscodes.h"
#include "http/response.h"
#include "helpers/bool/bool.h"
#include "helpers/strings/strings.h"
#include "http/headers/helpers.h"


#define FILESDIRECTORY "files/"

int handleRequest(SSL *ssl, char *requestbuf, int logfd, struct sockaddr_in source);
int getResource(const char *filename, char newResource[MAXPATH]);
//int setHTTPHeaders(responseHeaders headers, requestHeaders requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf);
char *getExtension(const char *filepath);
char *nullbyte(void);
int buildFilePath(const char *filename, char *filepath, unsigned int filepathsize);
//int fileNotFound(SSL *ssl);
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode);
//int fileNotAvailable(SSL *ssl);
int sendError(SSL *ssl, responseHeaders *headers, const char *statusCode, const requestHeaders *requestHeaders);
int sendGoodResponse(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders, const char *filepath, time_t lastModified, int filesize);

int handleConnection(int clientfd, SSL_CTX *ctx, int logfd, struct sockaddr_in source) {
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
				int res = handleRequest(ssl, requestbuf, logfd, source);
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
int handleRequest(SSL *ssl, char *requestbuf, int logfd, struct sockaddr_in source)//, ssize_t requestLength)
{
	printf("Response:\n");
	//int badReturn = 1;

	off_t filesizePure;
	int filesize;
	int res;
	char resource[MAXPATH];
	char filepath[MAXPATH];
	//char fileextension[MAXPATH];
	//char *headersbuf;
	//int headersLength;

	struct responseHeaders headers;
	initialiseResponseHeaders(&headers);

	struct requestHeaders requestHeaders;
	initialiseRequestHeaders(&requestHeaders);
	// parse headers
	char *statusCode = parseHeaders(&requestHeaders, requestbuf);
	if (statusCode != NULL) {
		// return with status code
		//setGenericHeaders(&headers, &requestHeaders);
		sendError(ssl, &headers, statusCode, &requestHeaders);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}
	// do not change requestbuf from this point on
	/*if (requestHeaders.method != METHOD_GET && requestHeaders.method != METHOD_HEAD) {
		// send method not supported or method not allowed
		setGenericHeaders(&headers, &requestheaders);
		sendResponse(ssl, STATUS_METHODNOTALLOWED, &headers, &requestHeaders, NULL, 0);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}*/
	logRequest(logfd, source, &requestHeaders);
	
	// Convert requested resource to actual resource name
	res = getResource(requestHeaders.resource, resource);
	if (res != 0) {
		fileNotFound(ssl, &headers, &requestHeaders, STATUS_NOTFOUND);
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}
	printf("Filename: %s\n", resource);

	// Convert resource name to file path
	//char *filepath = buildFilePath(resource);
	res = buildFilePath(resource, filepath, MAXPATH);
	if (res != 0) {
		fileNotFound(ssl, &headers, &requestHeaders, STATUS_URITOOLONG);
	}

	// Get file size and last modified time
	time_t lastModified;
	filesizePure = getFileDetails(filepath, &lastModified);
	if (filesizePure < 0) {
		switch (filesizePure) {
			case STAT_NOTREADABLE:
				fileNotFound(ssl, &headers, &requestHeaders, STATUS_NOTFOUND);
				break;
			case STAT_NOTFOUND:
				fileNotFound(ssl, &headers, &requestHeaders, STATUS_NOTFOUND);
				break;
			/*default:
				fileNotFound(ssl);
				return badReturn;*/
		}
		return requestHeaders.ConnectionKeep ? 1 : -1;
	}
	else if (filesizePure > INT_MAX) {
		res = fileNotFound(ssl, &headers, &requestHeaders, STATUS_NOTFOUND);
		return requestHeaders.ConnectionKeep ? res : -1;
	}
	filesize = (int) filesizePure;

	// check for 304 not modified
	if (lastModified <= requestHeaders.IfModifiedSince) {
		res = sendError(ssl, &headers, STATUS_NOTMODIFIED, &requestHeaders);
		// obv this isn't an error but what I mean is that there is no body
		return requestHeaders.ConnectionKeep ? res : -1;
	}

	// set headers based on collected information
	/*setGenericHeaders(&headers, &requestHeaders);
	setFileHeaders(&headers, &requestHeaders, lastModified, filesize, fileextension);*/
	//headersLength = setHTTPHeaders(responseHeaders, requestHeaders, lastModified, filesize, fileextension, &headersbuf);
	res = sendGoodResponse(ssl, &headers, &requestHeaders, filepath, lastModified, filesize);
	return requestHeaders.ConnectionKeep ? res : -1;
}

// name
int sendGoodResponse(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders, const char *filepath, time_t lastModified, int filesize)
{
	int res;
	// get file extension for MIME type
	char *fileextension = getExtension(filepath);
	char *headersbuf;
	int headersLength = buildHeaders(headers, requestHeaders, STATUS_OK, lastModified, filesize, fileextension, &headersbuf);
	if (headersLength < 0) {
		return 1;
	}
	free(fileextension);

	// Send headers and file
	if (requestHeaders->method == METHOD_GET) {
		res = sendResponse(ssl, headersbuf, headersLength, filepath, filesize);
	}
	else {
		res = sendResponse(ssl, headersbuf, headersLength, NULL, 0);
	}
	free(headersbuf);
	return 0;
}

int sendError(SSL *ssl, responseHeaders *headers, const char *statusCode, const requestHeaders *requestHeaders)
{
	int retval = 1;
	char *headersbuf;
	int headersLength = buildHeadersNoBody(headers, requestHeaders, statusCode, &headersbuf);
	if (headersLength > 0) {
		sendResponse(ssl, headersbuf, headersLength, NULL, 0);
		retval = 0;
	}
	free(headersbuf);
	return retval;
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
		char *resC = realpath(filepath, targetpath);
		if (resC == NULL) {
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

int buildFilePath(const char *filename, char *filepath, unsigned int filepathsize)
{
	char *prefix = FILESDIRECTORY;
	/*char *filepath = malloc(strlen(filename) + strlen(prefix) + 1);
	if (filepath == NULL) {
		perror("Can't malloc filepath");
		return NULL;
	}*/
	const char *strs[3] = {prefix, filename, NULL};
	filepath[0] = 0;
	int res = strlcat4(filepath, NULL, strs, filepathsize);
	if (res != 0) {
		return 1;
	}
	/*strcpy(filepath, prefix);
	strcat(filepath, filename);*/
	printf("Filepath: %s\n", filepath);
	return 0;
}

/*
* atm: returns 0 on success and 1 on failure
*/
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode) 
{
	char *headersbuf;
	int res;
	int headersLength;
	char filepath[MAXPATH];

	res = buildFilePath("404.html", filepath, MAXPATH);
	if (res != 0) { return 1; }
	time_t lastModified;
	off_t filesizePure = getFileDetails(filepath, &lastModified);
	if (filesizePure < 0 || filesizePure > INT_MAX) {
		fprintf(stderr, "Could not access 404.html to send\n");
		//headersLength = buildHeaders(headers, requestHeaders, STATUS_NOTFOUND, -1, -1, NULL, &headersbuf);
		headersLength = buildHeadersNoBody(headers, requestHeaders, statusCode, &headersbuf);
		if (headersLength < 0) { return 1; }
		res = sendResponse(ssl, headersbuf, headersLength, NULL, 0);
	}
	else {
		int filesize = (int) filesizePure;
		headersLength = buildHeaders(headers, requestHeaders, statusCode, -1, filesize, "html", &headersbuf);
		res = sendResponse(ssl, headersbuf, headersLength, filepath, filesize);
	}
	return res;
}


/*
int fileNotFound(SSL *ssl, responseHeaders *headers, const requestHeaders *requestHeaders) 
{
	int res;
	setGenericHeaders(headers, requestHeaders);
	char filepath[MAXPATH];
	res = buildFilePath("404.html", filepath, MAXPATH);
	if (res != 0) { return 1; }
	time_t modifiedSince;
	off_t filesizePure = getFileDetails(filepath, &modifiedSince);
	if (filesizePure < 0 || filesizePure > INT_MAX) {
		fprintf(stderr, "Could not access 404.html to send\n");
		res = sendResponse(ssl, STATUS_NOTFOUND, headers, NULL, 0);
	}
	else {
		int filesize = (int) filesizePure;
		setFileHeaders(headers, requestHeaders, 0, filesize, "html");
		res = sendResponse(ssl, STATUS_NOTFOUND, headers, filepath, filesize);
	}
	return res;
}
*/

/*
int fileNotAvailable(SSL *ssl) 
{
	return 0;
}
*/
