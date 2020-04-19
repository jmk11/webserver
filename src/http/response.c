
#include "response.h"
#include "headers/helpers.h"
#include "headers/response/contenttype.h"
#include "headers/response/custom.h"
#include "../files/files.h"

#define MAXSIZE 150

void setGenericHeaders(char *datebuf, unsigned int datebufsize, responseHeaders *headers, const requestHeaders *requestHeaders);


/*
* Return 0 on response sent successfully
* 1 on failed to produce headers
* 2 on failed to load file
* 3 on failed to send headers or file response
*/
int sendResponse(SSL *ssl, const char *headersbuf, int headersLength, const char *filepath, size_t filesize)
{
	int res;

	if (filepath != NULL) {
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
	else {
		//res = send(clientfd, headersbuf, headersLength, 0);
		res = SSL_write(ssl, headersbuf, headersLength);
		if (res != headersLength) {
			perror("Error send HTTP headers");
			return 3;
		}
	}
	return 0;
}

#define OFF_TDIGITS 19
#define OFF_TSTRMAX (OFF_TDIGITS+1)

// sets *headersbuf to malloced memory that must be freed
// returns header length
// if provided lastModified is 0, will not set last-Modified
// I have all this in one function because I want to keep the pointers in the headers struct on the stack
// because stack is per thread but heap is per process
// do not use headers after this function returns
int buildHeaders(responseHeaders *headers, const requestHeaders *requestHeaders, const char *statusCode, time_t lastModified, off_t fileLength, const char *fileExtension, char **headersbuf) 
{
	// this is a bad way of doing it as all the headers must be in ram in different strings at the same time
	int res;
	
	char datebuf[MAXSIZE];
	setGenericHeaders(datebuf, MAXSIZE, headers, requestHeaders);

	// maximum signed 64 bit number 9,223,372,036,854,775,807 -> 19 digits
	// content length
	if (fileLength != -1) {
		char contentLength[OFF_TSTRMAX];
		snprintf(contentLength, OFF_TSTRMAX, "%ld", fileLength);
		headers->ContentLength.value = contentLength;

		// content language and security headers
		addMyHeaders(headers);
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
		headers->ContentType.value = contentType.setContentType(TRUE, fileExtension);
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
	if (res == 0) {
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
