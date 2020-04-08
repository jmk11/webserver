#include <string.h>
#include <stdio.h>

#include "headers.h"
#include "helpers.h"
#include "headersbase.h"

#define HEADERSMAX 1024


int initialiseResponseHeaders(responseHeaders *headers) 
{
    *headers = responseHeadersBase;
    return 0;
}

int initialiseRequestHeaders(requestHeaders *headers) 
{
    *headers = requestHeadersBase;
    return 0;
}

/*
int addHeader(const char *header, char *headersstr, unsigned int headersstrlength)
{
    char *headersstrcur = headersstr;
    int res = strlcat3(headersstr, &headersstrcur, header, headersstrlength);
    if (res != 0) { return 1; }
    int res = strlcat3(headersstr, &headersstrcur, "\r\n", headersstrlength);
    if (res != 0) { return 1; }
    return 0;
    // could just do an snprintf on headersstrcur with remaining length
}*/

int produceHeaders(char *statusCode, char **headersstr, const responseHeaders *headers)
{
    // check that doesn't pass headersmax. if so, realloc
    *headersstr = malloc(HEADERSMAX);
    if (*headersstr == NULL) {
        perror("Can't malloc headersstr");
        return 1;
    }
    memset(*headersstr, 0, HEADERSMAX);
    char *headersstrcur = *headersstr;
    int res = strlcat3(*headersstr, &headersstrcur, "HTTP/1.1 ", HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, statusCode, HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    res = strlcat3(*headersstr, &headersstrcur, " OK\r\n", HEADERSMAX); // obv change this
    if (res != 0) { free(*headersstr); return 1; }
    /*int bytesWritten = snprintf(*headersstr, HEADERSMAX, "HTTP/1.1 %s OK\r\n", statusCode);
    char *headersstrcur = *headersstr + bytesWritten;*/

    // I'm relying on struct layout which you aren't supposed to do but I want to try this
    headerPair *pair = headers;
    unsigned int numPairs = sizeof(struct responseHeaders) / sizeof(headerPair);
    for (unsigned int i = 0; i < numPairs; i++, pair++) {
        if (pair->value != NULL) {
            res = strlcat3(*headersstr, &headersstrcur, pair->label, HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, ": ", HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, pair->value, HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            res = strlcat3(*headersstr, &headersstrcur, "\r\n", HEADERSMAX);
            if (res != 0) { free(*headersstr); return 1; }
            // can do this with one snprintf, but I would need to calculate the total number of bytes
            // I intend to write so I can know if it is less
        }
    }
    res = strlcat3(*headersstr, &headersstrcur, "\r\n", HEADERSMAX);
    if (res != 0) { free(*headersstr); return 1; }
    printf("\n%s\n", *headersstr);    

    return 0;
}

int parseHeaders(requestHeaders *headers, char *headersstr)
{
    // first line: replace first and second spaces with null byte
    // following lines: replace \r with null byte

    // resource:
    /*
    res = getFileName(requestbuf, &filename);
	if (res != 0) {
		fileNotFound(clientfd);
		// end this thread
		return 1;
	}
	
	res = sanitiseRequest(filename);
	if (res != 0) {
		fileNotFound(clientfd);
		//free(filename);
		return 1;
	}
	printf("Filename: %s\n", filename);
    */
    return 0;
}

// request is null terminated within BUFSIZ
// edits request to null terminate filename
// and returns pointer to where filename starts
int getResourceRequest(char *request, char **filename) 
{
	unsigned int i;
	char *method = "GET /";
	unsigned int methodlen = strlen(method);
	if (strlen(request) < methodlen) {
		return 1;
	}
	for (i = 0; i < methodlen; i++) {
		if (request[i] != method[i]) {
			return 1;
		}
	}
	// now we are pointing at char after /
	// could be null
	*filename = &(request[i]);
	// turn first space into null:
	while (request[i] != ' ' && request[i] != 0) { i++; }
	request[i] = 0;
	/*
	for (; request[i] != 0; i++) {
		if (request[i] == ' ') { 
			request[i] = 0; 
			break; 
		}
	}*/
	
	/*
	int nitems = sscanf(request, "GET %""499s", request1);
    if (nitems != 1)
    {
    	//ferror()
    	perror("Error sscanf");
        return 1;
    }

    // stupid scanf skips leading whitespaces
    // so if no arg, I just get "HTTP......"
    // so I'm including the / and if there is an arg it will be
    // /word,
    // otherwise "/\0"
    *filename = &(request1[1]);
    */
    
	return 0;
}

void freeHeadersstr(char *headersstr) {
    free(headersstr);
}