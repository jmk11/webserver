#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include "uid.h"
#include "constants.h"
#include "helpers.h"
#include "common.h"
#include "headers.h"

#define RESPSIZE 300
#define DOMAINSIZE 100
#define REQSIZE 300
#define DOMAINFILE "config/domain.txt"

int readDomain(char domain[DOMAINSIZE]);
int handleConnection(int clientfd, const char *domain);


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
	//char *filebuf;
    //char *response = "HTTP/1.1 301 Moved Permanently\r\nLocation: https://localhost\r\n\r\n";
	//char response[RESPSIZE];
	char httpsdomain[DOMAINSIZE];
	//char requestbuf[REQSIZE];
	//char *nullbyte = "\0";
	//char *resource;

	int res = readDomain(httpsdomain);
	if (res != 0) {
		fprintf(stderr, "Can't read domain name from config file. Exiting\n");
		return 1;
	}

	printf("Entering accept loop..\n");
	while (1) {
		clientfd = accept(serverfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		if (clientfd < 0) {
			perror("HTTP accept clientfd < 0");
			fprintf(stderr, "Continuing to next loop...\n");
		}
		else {
			logSource(clientAddr, "HTTP ");
			handleConnection(clientfd, httpsdomain);
		}

		close(clientfd);
		// what do if can't close client socket?
	}
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
			fprintf(stderr, "Couldn't retrive resource from request\n");
			resource = nullbyte;
		}
	}

	// append requested resource to domain name
	snprintf(response, RESPSIZE, "HTTP/1.1 301 Moved Permanently\r\nLocation: %s/%s\r\n\r\n", domain, resource);
	send(clientfd, response, strlen(response), 0);

	return 0;
}

// can generalise some of this and readCert?
int readDomain(char domain[DOMAINSIZE])
{
	int fd = open(DOMAINFILE, O_RDONLY);
    if (fd < 0) {
        perror("Can't open domain config file\n");
        return 1;
    }
    // obviously deal with size etc.
    int bytesRead = read(fd, domain, DOMAINSIZE-1);
    if (bytesRead <= 0) {
        perror("Can't read comain config file");
        return 1;
    }
	domain[bytesRead] = 0;
	if (domain[bytesRead-1] == '\n') {
		domain[bytesRead-1] = 0;
	}
	/*for (unsigned int i = 0; i < bytesRead; i++) {

	}*/
    close(fd);
	return 0;
}


