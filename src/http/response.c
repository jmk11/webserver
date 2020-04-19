
#include "response.h"
#include "headers/helpers.h"
#include "headers/response/contenttype.h"
#include "headers/response/custom.h"
#include "../files/files.h"

#define MAXSIZE 150

/*
* Return 0 on response sent successfully
* 1 on failed to produce headers
* 2 on failed to load file
* 3 on failed to send headers or file response
*/
int sendResponse(SSL *ssl, const char *statusCode, const responseHeaders *headers, enum Method method, const char *filepath, size_t filesize)
{
	int res;
	char *headersbuf;
	int headersLength = produceHeaders(statusCode, &headersbuf, headers);
	if (headersLength == -1) {
		return 1;
	}

	if (method == METHOD_GET && filepath != NULL) {
		char *filebuf;
		res = loadRequestedFile(filepath, &filebuf, filesize);
		if (res != 0) {
			/*responseHeaders headers404;
			initialiseResponseHeaders(&headers404);
			fileNotFound(ssl, &headers404, requestHeaders);*/
			return 2;
		}
		// won't scale with large file size
		
		//res = send(clientfd, headersbuf, headersLength, 0);
		res = SSL_write(ssl, headersbuf, headersLength);
		free(headersbuf);
		free(filebuf);
		if (res != headersLength) {
			perror("Error send HTTP headers");
			return 3;
		}
		//res = send(clientfd, filebuf, fileLength, 0);
		res = SSL_write(ssl, filebuf, filesize);
		if (res != filesize) {
			perror("Error send file");
			return 3;
		}
	}
	else /* if HEAD */ {
		//res = send(clientfd, headersbuf, headersLength, 0);
		res = SSL_write(ssl, headersbuf, headersLength);
		free(headersbuf);
		if (res != headersLength) {
			perror("Error send HTTP headers");
			return 3;
		}
	}
	return 0;
}

#define OFF_TDIGITS 19
#define OFF_TSTRMAX (OFF_TDIGITS+1)


/*
* Sets date and DNT
*/
void setGenericHeaders(responseHeaders *headers, const requestHeaders *requestHeaders)
{
	// date
	char datebuf[MAXSIZE];
	int res = getHTTPDate(datebuf, MAXSIZE);
	if (res == 0) {
		headers->Date.value = datebuf;
	}

	// DNT
	if (requestHeaders->DNT == DNT1) {
		headers->Tk.value = "N";
	}
}

/* 
* Sets content length, last modified, content type, content language, and security headers
* If provided lastModified is 0, will not set last modified
*/
void setFileHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, time_t lastModified, off_t fileLength, const char *fileExtension) 
{
	int res;
	// maximum signed 64 bit number 9,223,372,036,854,775,807 -> 19 digits
	// content length
	char contentLength[OFF_TSTRMAX];
	snprintf(contentLength, OFF_TSTRMAX, "%ld", fileLength);
	headers->ContentLength.value = contentLength;

	// this is a bad way of doing it as all the headers must be in ram in different strings at the same time

	if (lastModified != 0) {
		char lmbuf[MAXSIZE];
		res = timetoHTTPDate(lastModified, lmbuf, MAXSIZE);
		if (res == 0) {
			headers->LastModified.value = lmbuf;
		}
	}

	// content type
	//setContentType(TRUE, fileExtension, &(headers->ContentType.value));
	headers->ContentType.value = contentType.setContentType(TRUE, fileExtension);

	addMyHeaders(headers);

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
