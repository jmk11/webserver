#include <stdio.h>
//#include <sys/socket.h>
#include <netinet/in.h>
//#include <fcntl.h>
//#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "helpers.h"
#include "constants.h"

#define UID 1002


int buildSocket(unsigned short port);
int dropPermissions(unsigned short goaluid);
int handleRequest(int clientfd, char *requestbuf, ssize_t requestLength);

int getFileName(char *request, char **filename);
int sanitiseRequest(char *filename);
int loadRequestedFile(const char *filename, char **filebuf, off_t *fileLength);
int loadHTTPHeaders(off_t fileLength, char **headersbuf, unsigned long *headersLength);

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
			char response[BUFSIZ] = "HTTP/1.1 200 OK\r\n"
									"Content-Type: text/html; charset=utf-8\r\n\r\n"
									"<html><title>Justin</title><body><h3><center>Under Construction</br></br></br></br></br>Coming Christmas 2002!</h3></center></body></html>";
			
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
	off_t fileLength;
	unsigned long headersLength;
	int res;
	
	res = getFileName(requestbuf, &filename);
	if (res != 0) {
		fileNotFound(clientfd);
		// end this thread
		return 1;
	}
	printf("Filename: %s", filename);
	
	res = sanitiseRequest(filename);
	if (res != 0) {
		fileNotFound(clientfd);
		//free(filename);
		return 1;
	}
	res = loadRequestedFile(filename, &filebuf, &fileLength);
	if (res != 0) {
		fileNotFound(clientfd);
		//free(filename);
		return 1;
	}
	// won't scale with large file size
	//free(filename);
	
	res = loadHTTPHeaders(fileLength, &headersbuf, &headersLength);
	res = send(clientfd, headersbuf, headersLength, 0);
	if (res != headersLength) {
		perror("Error send HTTP headers");
		free(headersbuf);
		free(filebuf);
		return 1;
	}
	
	res = send(clientfd, filebuf, fileLength, 0);
	if (res != fileLength) {
		perror("Error send file");
		free(headersbuf);
		free(filebuf);
		return 1;
	}
	free(headersbuf);
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
	
	if (filename[0] == 0) {
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

int sanitiseRequest(char *filename) 
{
	return 0;
}

int loadRequestedFile(const char *filename, char **filebuf, off_t *fileLength) 
{ 
	return 0;
}

int loadHTTPHeaders(off_t fileLength, char **headersbuf, unsigned long *headersLength) 
{
	return 0;
}

int fileNotFound(int clientfd) 
{
	char *response = "404";
	int res = send(clientfd, response, strlen(response), 0);
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
