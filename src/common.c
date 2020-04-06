#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <string.h>

#include "constants.h"
#include "helpers.h"


void printSource(struct sockaddr_in addrStruct, const char *prefix)
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
	if (prefix != NULL) {
		printf ("%s Connection from %s:%hu\n", prefix, addrstr, addrStruct.sin_port);
	}
	else {
		printf ("Connection from %s:%hu\n", addrstr, addrStruct.sin_port);
	}
}

int logSource(int logfd, struct sockaddr_in addrStruct)
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
	unsigned int bufsize = 100;
	char buf[bufsize];
	snprintf(buf, bufsize, "Connection from %s:%hu\n", addrstr, addrStruct.sin_port);
	int bytesWritten = write(logfd, buf, strlen(buf));
	if (bytesWritten != strlen(buf)) {
		if (bytesWritten < 0) {
			perror("Couldn't write to log file");
		}
		else {
			fprintf(stderr, "Couldn't write desired bytes to log file\n");
		}
		return 1;
	}
	return 0;
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