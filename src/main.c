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


int buildSocket(unsigned short port);
int dropPermissions(unsigned short goaluid);
int handleRequest(int clientfd, char *requestbuf, ssize_t requestLength);

int getFileName(char *request, char **filename);
int sanitiseRequest(const char *filename);
int loadRequestedFile(const char *filename, char **filebuf, off_t *fileLength);
int loadHTTPHeaders(off_t fileLength, char *filetype, char **headersbuf, unsigned long *headersLength);

char *getExtension(const char *filename);

int fileNotFound(int clientfd);
int fileNotAvailable(int clientfd);

int main(int argc, char **argv)
{
	printf("uid at start: %d\n", getuid());
	
	unsigned short port;
	if (argc == 2) {
		port = getPort(argv[1]);
	} 
	else {
		port = 80;
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
	char requestbuf[BUFSIZ];
	ssize_t requestLen;
	//char *filebuf;
	
	printf("Entering accept loop..\n");
	while (1) {
		clientfd = accept(serverfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		if (clientfd < 0) {
			perror("accept clientfd < 0");
		}
		else {
			requestLen = recv(clientfd, requestbuf, BUFSIZ-1, 0);
			if (requestLen < 0) {
				perror("Error recv");
			}
			else {
				requestbuf[requestLen] = 0;
				printf("%s\n", requestbuf);
				handleRequest(clientfd, requestbuf, requestLen);
			}
			close(clientfd);
			// what do if can't close client socket?
		}
	}
	
	Close(serverfd);
	return 0;	
}

int handleRequest(int clientfd, char *requestbuf, ssize_t requestLength)
{
	char *filename = NULL;
	char *filebuf = NULL;
	char *headersbuf = NULL;
	off_t fileLength = 0;
	unsigned long headersLength = 0;
	int res;
	
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
	
	res = loadRequestedFile(filename, &filebuf, &fileLength);
	if (res != 0) {
		fileNotFound(clientfd);
		//free(filename);
		return 1;
	}
	// won't scale with large file size
	//free(filename);
	char *fileextension = getExtension(filename);
	
	res = loadHTTPHeaders(fileLength, fileextension, &headersbuf, &headersLength);
	res = send(clientfd, headersbuf, headersLength, 0);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send HTTP headers");
		free(filebuf);
		return 1;
	}
	
	res = send(clientfd, filebuf, fileLength, 0);
	if (res != fileLength) {
		perror("Error send file");
		free(filebuf);
		return 1;
	}
	free(filebuf);
	return 0;
}

int dropPermissions(unsigned short goaluid) {
	setuid(goaluid);
	printf("uid after drop: %d %d\n", getuid(), geteuid());
	setuid(0);
	printf("uid fter setuid: %d %d\n", getuid(), geteuid());
	seteuid(0);
	printf("uid after seteuid: %d %d\n", getuid(), geteuid());
	if (getuid() != goaluid || geteuid() != goaluid) {
		exit(PERMISSIONSERROR);
	}
	return 0;
	// does this check introduce security problems by calling setuid and seteuid 0?
}

int buildSocket(unsigned short port) 
{
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	(serverAddr).sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	Bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	Listen(sockfd, 5);
	return sockfd;
}

// request is null terminated within BUFSIZ
// edits request to null terminate filename
// and returns pointer to where filename starts
int getFileName(char *request, char **filename) 
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
	
	if ((*filename)[0] == 0) {
		strncpy(*filename, "index.html", BUFSIZ-methodlen); // check this isn't 1 byte overflow
	}
	
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

// filename length < bufsiz
int sanitiseRequest(const char *filename) 
{
	for (unsigned int i = 0; filename[i+1] != 0; i++) {
		if (filename[i] == '.' && filename[i+1] == '.') {
			return 1;
		}
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
	printf("%s\n\n\n", filepath);
	
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
	headers headers;
	initialiseHeaders(&headers);

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

	headers.ContentLanguage.value = "en";

	//securityheaders.com:
	headers.XContentTypeOptions.value = "nosniff";
	headers.XFrameOptions.value = "SAMEORIGIN";
	headers.ContentSecurityPolicy.value = "default-src 'none'";

	if (fileExtension != NULL) {
		if (strcmp(fileExtension, "jpg") == 0) {
			headers.ContentType.value = "image/jpeg";
		}
		else {
			headers.ContentType.value = "text/html; charset=UTF-8";
		}
	}
	
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

int fileNotFound(int clientfd) 
{
	unsigned int headersMax = 500;
	char *headersbuf = malloc(headersMax);
	char *headersbufcur = headersbuf;
	strlcat3(headersbuf, &headersbufcur, "HTTP/1.1 404 Not Found\r\n", headersMax);
	strlcat3(headersbuf, &headersbufcur, "\r\n", headersMax);
	unsigned int headersLength = headersbufcur - headersbuf;
	int res = send(clientfd, headersbuf, headersLength, 0);
	free(headersbuf);
	if (res != headersLength) {
		perror("Error send 404 header");
		return 1;
	}

	char *response = "404";
	res = send(clientfd, response, strlen(response), 0);
	if (res != strlen(response)) {
		perror("Error send file");
		return 1;
	}
	return 0;
}
int fileNotAvailable(int clientfd) 
{
	return 0;
}
