/*
* Separate application serving HTTP redirects to HTTPS server. Domain defined in config/domain.txt
*/

#include <stdio.h>
// #include <netinet/in.h>
#include <string.h>
#include <unistd.h>
// #include <stdlib.h>
#include <fcntl.h>

#include "uid.h"
#include "constants.h"
#include "common.h"
#include "wrappers/wrappers.h"
#include "helpers/bool/bool.h"

#define RESPSIZE 300
#define REQSIZE 300
#define LOGFILE "logs/HTTPlog.txt"

int handleConnection(int clientfd, const char *domain);
int getResourceRequest(char *request, char **filename);

int main(int argc, char **argv)
{
	printf("uid at start: %d\n", getuid());
	printf("euid at start: %d\n", geteuid());
	
	unsigned short port;
	if (argc == 2) {
		int res = getPort(argv[1], &port);
		if (res != 0) {
			fprintf(stderr, "Exiting.\n");
			return ARGSERROR;
		}
	} 
	else {
		port = 80;
	}
	printf("Using port number %hu\n", port);
	
	int serverfd = buildSocket(port);
	
	// after binding port 80, can drop permissions
	dropPermissions(UID);

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientfd;
	char httpsdomain[DOMAINSIZE];
	bool fileLogging = TRUE;

	int res = readDomain(httpsdomain);
	if (res != 0) {
		fprintf(stderr, "Can't read domain name from config file. Exiting\n");
		return 1;
	}

	int logfd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0660);
	if (logfd < 0) {
		perror("Cannot open log file for writing");
		fprintf(stderr, "Continuing execution without logging to file..");
		fileLogging = FALSE;
	}

	printf("Entering accept loop..\n");
	while (1) {
		clientfd = accept(serverfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		if (clientfd < 0) {
			perror("HTTP accept clientfd < 0");
			fprintf(stderr, "Continuing to next loop...\n");
		}
		else {
			printSource(clientAddr, "HTTP ");
			if (fileLogging) {
				logSource(logfd, clientAddr);
			}
			handleConnection(clientfd, httpsdomain);
		}

		fsync(logfd);
		close(clientfd);
		// what do if can't close client socket?
	}
	
	// I think it's a problem that the log file doesn't get closed?
	// Also it is being created with really weird permissions...
	close(logfd);
	close(serverfd);
	return 0;	
}

int handleConnection(int clientfd, const char *domain)
{
	char *nullbyte = "\0";
	char *resource;
	char requestbuf[BUFSIZ];
	char response[RESPSIZE];

	int requestLen = recv(clientfd, requestbuf, BUFSIZ-1, 0);
	if (requestLen <= 0) {
		fprintf(stderr, "HTTP read error\n");
		resource = nullbyte;
	}
	else {
		requestbuf[requestLen] = 0;
		printf("HTTP: %s\n", requestbuf);
		int res = getResourceRequest(requestbuf, &resource);
		if (res != 0) {
			fprintf(stderr, "HTTP Couldn't retrieve resource from request\n");
			resource = nullbyte;
		}
	}

	// append requested resource to domain name
	snprintf(response, RESPSIZE, "HTTP/1.1 301 Moved Permanently\r\nLocation: %s%s\r\n\r\n", domain, resource);
	send(clientfd, response, strlen(response), 0);

	return 0;
}

// request is null terminated within BUFSIZ
// edits request to null terminate filename
// and returns pointer to where filename starts
int getResourceRequest(char *request, char **filename) 
{
	for (unsigned int i = 0; request[i] != 0; i++) {
		// you can use strchr for this
		if (request[i] == '/') {
			*filename = &(request[i]);
			// turn first space into null:
			while (request[i] != ' ' && request[i] != 0) { i++; }
			request[i] = 0;
			return 0;
		}
	}
	return 1;
}


