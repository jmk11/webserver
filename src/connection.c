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
#include "helpers/helpers.h"
//#include "constants.h"
#include "headers/headers.h"
#include "headers/custom.h"
//#include "common.h"
#include "headers/contenttype.h"
#include "wrappers.h" // only needs this for error codes, this isn't right
#include "files.h"

#define FILESDIRECTORY "files/"

int handleRequest(SSL *ssl, char *requestbuf);//, ssize_t requestLength);
char *buildFilePath(const char *filename);
int getResource(const char *filename, char newResource[MAXPATH]);
int loadHTTPHeaders(requestHeaders requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf);
char *getExtension(const char *filepath);
char *nullbyte(void);
int fileNotFound(SSL *ssl);
int fileNotAvailable(SSL *ssl);

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
* Return 0 on good and continue connection
* Return 1 on bad and continue connection
* Return -1 on finish connection
*/
int handleRequest(SSL *ssl, char *requestbufedit)//, ssize_t requestLength)
{
	printf("Response:\n");
	int badReturn = 1;

	//char *filename = NULL;
	char *headersbuf = NULL;
	//off_t filesize;
	off_t filesizePure;
	int filesize;
	int headersLength;
	// the whole point of headersLength is eventually to be used as int parameter to ssl_write
	// but along the way i would like to be able to assume it is unsigned
	// so what to do?
	int res;
	char resource[MAXPATH];

	struct requestHeaders requestHeaders;
	initialiseRequestHeaders(&requestHeaders);
	res = parseHeaders(&requestHeaders, requestbufedit);
	if (res != 0) {
		// return with status code
		fileNotFound(ssl);
		return -1;
	}
	if (requestHeaders.ConnectionKeep == FALSE) {
		badReturn = -1;
	}
	const char* const requestbuf = requestbufedit;
	if (requestHeaders.method != METHOD_GET && requestHeaders.method != METHOD_HEAD) {
		// send method not supported or method not allowed
		return badReturn;
	}
	struct responseHeaders headers;
	initialiseResponseHeaders(&headers);
	
	/*res = getResourceRequest(requestbuf, &filename);
	if (res != 0) {
		fileNotFound(ssl);
		// end this thread
		return 1;
	}*/
	
	res = getResource(requestHeaders.resource, resource);
	if (res != 0) {
		fileNotFound(ssl);
		return badReturn;
	}
	printf("Filename: %s\n", resource);

	char *filepath = buildFilePath(resource);
	time_t lastModified;
	filesizePure = getFileDetails(filepath, &lastModified);
	if (filesizePure < 0) {
		switch (filesizePure) {
			case STAT_NOTMODIFIED:
				break; // return 304
			case STAT_NOTREADABLE:
				(void*) NULL; // return 404 prob
			default:
				fileNotFound(ssl);
				free(filepath);
				return badReturn;
		}
	}
	else if (filesizePure > INT_MAX) {
		fileNotFound(ssl);
		free(filepath);
		return badReturn;
	}
	filesize = (int) filesizePure;
	if (lastModified <= requestHeaders.IfModifiedSince) {
		// 304
		headersbuf = malloc(100);
		headersLength = snprintf(headersbuf, 100, "HTTP/1.1 304 Not Modified\r\n\r\n");
		res = SSL_write(ssl, headersbuf, headersLength);
		free(headersbuf);
		if (res != headersLength) {
			perror("Error send 304 header");
			return 1;
		}
		return requestHeaders.ConnectionKeep ? 0 : -1;
	}

	char *fileextension = getExtension(filepath);

	headersLength = loadHTTPHeaders(requestHeaders, lastModified, filesize, fileextension, &headersbuf);
	free(fileextension);
	if (headersLength == -1) {
		return badReturn;
	}

	if (requestHeaders.method == METHOD_GET) {
		char *filebuf;
		res = loadRequestedFile(filepath, &filebuf, filesize);
		if (res != 0) {
			fileNotFound(ssl);
			free(filepath);
			return badReturn;
		}
		free(filepath);
		// won't scale with large file size
		
		//res = send(clientfd, headersbuf, headersLength, 0);
		res = SSL_write(ssl, headersbuf, headersLength);
		free(headersbuf);
		if (res != headersLength) {
			perror("Error send HTTP headers");
			free(filebuf);
			return badReturn;
		}
		//res = send(clientfd, filebuf, fileLength, 0);
		res = SSL_write(ssl, filebuf, filesize);
		if (res != filesize) {
			perror("Error send file");
			free(filebuf);
			return badReturn;
		}
		free(filebuf);
	}
	else {
		free(filepath);
		//res = send(clientfd, headersbuf, headersLength, 0);
		res = SSL_write(ssl, headersbuf, headersLength);
		free(headersbuf);
		if (res != headersLength) {
			perror("Error send HTTP headers");
			return badReturn;
		}
	}

	return requestHeaders.ConnectionKeep ? 0 : -1;
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

#define OFF_TDIGITS 19
#define OFF_TSTRMAX (OFF_TDIGITS+1)

// sets *headersbuf to malloced memory that must be freed
// returns header length
int loadHTTPHeaders(requestHeaders requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf) 
{
	responseHeaders headers;
	initialiseResponseHeaders(&headers);
	// remove this initialisation and take initialised responseHeaders paramater

	// maximum signed 64 bit number 9,223,372,036,854,775,807 -> 19 digits
	// content length
	char contentLength[OFF_TSTRMAX];
	snprintf(contentLength, OFF_TSTRMAX, "%ld", fileLength);
	headers.ContentLength.value = contentLength;

	// date
	char datebuf[MAXLENGTH];
	int res = getHTTPDate(datebuf, MAXLENGTH);
	if (res == 0) {
		headers.Date.value = datebuf;
	}
	// this is a bad way of doing it as all the headers must be in ram in different strings at the same time

	char lmbuf[MAXLENGTH];
	res = timetoHTTPDate(lastModified, lmbuf, MAXLENGTH);
	if (res == 0) {
		headers.LastModified.value = lmbuf;
	}
	
	addMyHeaders(&headers);
	if (requestHeaders.DNT == DNT1) {
		headers.Tk.value = "N";
	}

	// content type
	setContentType(TRUE, fileExtension, &(headers.ContentType.value));
	
	return produceHeaders("200 OK", headersbuf, &headers);

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
	/*headersbuf[0] = 0;
	char *headersbufcur = headersbuf;
	strlcat3(headersbuf, &headersbufcur, "HTTP/1.1 404 Not Found\r\n\r\n", headersMax);
	int headersLength = headersbufcur - headersbuf;*/
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
}
int fileNotAvailable(SSL *ssl) 
{
	return 0;
}
