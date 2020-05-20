#include <unistd.h>

#include "response.h"
#include "contenttype.h"
#include "custom.h"
#include "../helpers.h"

#define MAXSIZE 150
// same as MAXPATH so should move to one imported definition
// or should connection and response be like independent modules
#define FILEBUFSIZE 1000000
static const unsigned long streamcutoff = 100*1024*1024;

int buildHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf);
int buildHeadersNoBody(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, char **headersbuf);
void setGenericHeaders(char *datebuf, unsigned int datebufsize, responseHeaders *headers, const requestHeaders *requestHeaders);

// I would like the interface to be
// sendResponse(ssl, const *statusCode, const *responseHeaders, const char *body, int bodylength)
// sendResponseBuffered(ssl, const *statusCode, const *responseHeaders, int fd)
// sendResponseNoBody(ssl, const *statusCode, const *responseHeaders)
// addFileHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension)
// but the problem is the addFileHeaders - the pointers used when produceHeaders() is called need to be malloced, or on the stack of the caller
// If I want to not malloc() them, then handleRequest will need to have the buffers on its stack and pass them to addFileHeaders()
// this is a broader problem with the idea that I want to be able to add headers to the requestHeaders struct at any point in the process - the char arrays need to be on handleRequests's stack
// content length is essential, content type is basically essential, but lastModified isn't. The ones that are essential I could add within sendResponse
// but others it would be better if handleRequest added them

// what about a function that adds headers and then sends
// addHeadersAndSend
// addHeadersAndSendBuffered
// addHeadersAndSendNoBody
// that is the current way :'D
// I just don't like all the required arguments on the send functions

// alternative is 
// sendResponse(ssl, const char *headersbuf, int headerslength, const char *body, int bodylength)
// sendResponseBuffered(ssl, const char *headersbuf, int fd)
// sendResponseNoBody(ssl, const char *headersbuf)
// char *buildHeaders(const *responseHeaders, char **headersbuf)

/*
in handlerequest:
	responseHeaders.something = ...

*/

/* 
 * Add standard headers and send headers and body
 * Return 0 on success, other on failure
 * bodylength: a meaningful value is necessary
 * lastModified: if 0, won't add lastModified header
 * File extension: used to set mime type. If it is NULL, mimetype will be text/plain // TODO: verify that
 * what if give the filestat struct
 */
int sendResponse(SSL *ssl, const char *statusCode, responseHeaders *headers, const requestHeaders *requestHeaders, const char *body, int bodylength, time_t lastModified, const char *fileextension)
{
	int res;

	// build headers
	char *headersbuf;
	int headersLength = buildHeaders(headers, requestHeaders, statusCode, lastModified, bodylength, fileextension, &headersbuf);
	if (headersLength < 0) {
		return 1;
	}

	// Send headers and file
	res = SSL_write(ssl, headersbuf, headersLength);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send HTTP headers");
		return 3;
	}
	res = SSL_write(ssl, body, bodylength);
	if (res != bodylength) {
		perror("Error send file");
		return 3;
	}

	return 0;
}

/* 
 * Add standard headers and send headers and body
 * Return 0 on success, other on failure
 * Reads and sends body of filesize bytes from fd, n bytes at a time
 * bodylength: a meaningful value is necessary
 * lastModified: if 0, won't add lastModified header
 * File extension: used to set mime type. If it is NULL, mimetype will be text/plain // TODO: verify that
 */
int sendResponseBuffered(SSL *ssl, const char *statusCode, responseHeaders *headers, const requestHeaders *requestHeaders, int fd, int filesize, time_t lastModified, const char *fileextension)
{
	int res;

	// build headers
	char *headersbuf;
	int headersLength = buildHeaders(headers, requestHeaders, statusCode, lastModified, filesize, fileextension, &headersbuf);
	if (headersLength < 0) {
		return 1;
	}

	// Send headers
	res = SSL_write(ssl, headersbuf, headersLength);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send HTTP headers");
		return 3;
	}

	// send file
	char filebuf[FILEBUFSIZE];
	ssize_t bytesRead;
	unsigned int count = 0;
	while ((bytesRead = read(fd, filebuf, FILEBUFSIZE)) > 0) {
		res = SSL_write(ssl, filebuf, bytesRead);
		if (res != bytesRead) {
			perror("Error send file");
			return 3;
		}
		if (count++ % 50 == 0) {
			printf("%d, %d bytes\n", count, count*FILEBUFSIZE);
		}
	}
	printf("%d, %d bytes\n", count, count*FILEBUFSIZE);


	return 0;
}

/* 
 * Add standard headers and send headers
 * Return 0 on success, other on failure
 */
int sendResponseNoBody(SSL *ssl, responseHeaders *headers, const char *statusCode, const requestHeaders *requestHeaders)
{
	char *headersbuf;
	int headersLength = buildHeadersNoBody(headers, requestHeaders, statusCode, &headersbuf);
	if (headersLength < 0) {
		return 1;
	}
	int res = SSL_write(ssl, headersbuf, headersLength);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send HTTP headers");
		return 3;
	}
	return 0;
}

#define OFF_TDIGITS 19
#define OFF_TSTRMAX (OFF_TDIGITS+1)

/* 
 * Sets headers content length, last modified, content type, date +
 * and then builds headers into malloced string
 * sets *headersbuf to malloced memory that must be freed
 * Returns header length
 * if provided lastModified is 0, will not set last-Modified
 * I have all this in one function because I want to keep the pointers in the headers struct on the stack
 * because stack is per thread but heap is per process
 * do not use headers after this function returns
*/
int buildHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf) 
{
	// this is a bad way of doing it as all the headers must be in ram in different strings at the same time
	int res;
	
	char datebuf[MAXSIZE];
	setGenericHeaders(datebuf, MAXSIZE, headers, requestHeaders);

	// maximum signed 64 bit number 9,223,372,036,854,775,807 -> 19 digits
	// content length
	if (fileLength >= 0) {
		char contentLength[OFF_TSTRMAX];
		snprintf(contentLength, OFF_TSTRMAX, "%ld", fileLength);
		headers->ContentLength.value = contentLength;

		// content language and security headers
		addMyHeaders(headers);
		if ((unsigned long) fileLength > streamcutoff) {
			headers->ContentDisposition.value = "attachment";
		}
	}
	
	// last modified
	if (lastModified != -1) {
		char lmbuf[MAXSIZE];
		res = timetoHTTPDate(lastModified, lmbuf, MAXSIZE);
		if (res == 0) {
			headers->LastModified.value = lmbuf;
		}
	}

	// content type
	//setContentType(TRUE, fileExtension, &(headers->ContentType.value));
	if (fileExtension != NULL) {
		headers->ContentType.value = setContentType(fileExtension);
	}
	
	return produceHeaders(statusCode, headersbuf, headers);
}

/*
* Sets date and DNT
*/
void setGenericHeaders(char *datebuf, unsigned int datebufsize, responseHeaders *headers, const requestHeaders *requestHeaders)
{
	// date
	int res = getHTTPDate(datebuf, datebufsize);
	if (res > 0) {
		headers->Date.value = datebuf;
	}
	// DNT
	if (requestHeaders->DNT == DNT1) {
		headers->Tk.value = "N";
	}
}

int buildHeadersNoBody(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, char **headersbuf) 
{
	char datebuf[MAXSIZE];
	setGenericHeaders(datebuf, MAXSIZE, headers, requestHeaders);
	return produceHeaders(statusCode, headersbuf, headers);
}

/*
* Sets date and DNT
*/
/*
void setGenericHeaders(responseHeaders *headers, const requestHeaders *requestHeaders)
{
	
}
*/

/* 
* Sets content length, last modified, content type, content language, and security headers
* If provided lastModified is 0, will not set last modified
*/
/*
void setFileHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension) 
{
	
}
*/

/*
if (filepath != NULL) {
	char *filebuf;
	res = loadRequestedFile(filepath, &filebuf, filesize);
	if (res != 0) {
		//responseHeaders headers404;
		//initialiseResponseHeaders(&headers404);
		//fileNotFound(ssl, &headers404, requestHeaders);
		return 2;
	}
	// won't scale with large file size
*/
