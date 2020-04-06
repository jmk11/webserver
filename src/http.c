#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "uid.h"
#include "constants.h"
#include "helpers.h"

void logSource(struct sockaddr_in addrStruct);
int dropPermissions(unsigned short goaluid);
int buildSocket(unsigned short port);

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
    char *response = "HTTP/1.1 301 Moved Permanently\r\nLocation: https://site.jmack.me\r\n\r\n";
	
	printf("Entering accept loop..\n");
	while (1) {
		clientfd = accept(serverfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		if (clientfd < 0) {
			perror("accept clientfd < 0");
		}
		logSource(clientAddr);
        send(clientfd, response, strlen(response), 0);
		close(clientfd);
		// what do if can't close client socket?
	}
	return 0;	
}

void logSource(struct sockaddr_in addrStruct)
{
	// from a 1521 sample code
	char addrstr[16];
	in_addr_t addrNum = ntohl (addrStruct.sin_addr.s_addr);
	snprintf (
		addrstr, 16, "%u.%u.%u.%u",
		addrNum >> 24 & 0xff,
		addrNum >> 16 & 0xff,
		addrNum >>  8 & 0xff,
		addrNum       & 0xff
	);
	printf ("HTTP Connection from %s:%hu\n", addrstr, addrStruct.sin_port);
}

int dropPermissions(unsigned short goaluid) {
	printf("Dropping permissions after binding...\n");
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

