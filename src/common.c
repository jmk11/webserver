// #include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "common.h"
#include "constants.h"
#include "wrappers/wrappers.h"

#define MAXPORT 65535


/*
* Print "[prefix] Connection from [IP]:[port]\n"
* prefix can be NULL - no prefix will be printed
*/
void printSource(struct sockaddr_in addrStruct, const char *prefix)
{
	char addrstr[16];
	stringIP(addrstr, addrStruct.sin_addr.s_addr);

	if (prefix != NULL) {
		printf("%s Connection from %s:%hu\n", prefix, addrstr, addrStruct.sin_port);
	}
	else {
		printf("Connection from %s:%hu\n", addrstr, addrStruct.sin_port);
	}
}

/*
* Write "Connection from [IP]:[Port]\n" to logfd
* Returns 0 on success, 1 on failure
*/
int logSource(int logfd, struct sockaddr_in addrStruct)
{
	char addrstr[16];
	stringIP(addrstr, addrStruct.sin_addr.s_addr);
	unsigned int bufsize = 100;
	char buf[bufsize]; // variable length array?
	snprintf(buf, bufsize, "Connection from %s:%hu\n", addrstr, addrStruct.sin_port);
	
	int bytesWritten = write(logfd, buf, strlen(buf));
	// if strlen(buf) was the unsigned equivalent of -1, and it got cast to signed for this comparison,
	// and write returned -1, then bytesWritten == strlen would be true
	// would add check that strlen(buf) is <= INT_MAX, but ofc that would be a waste of execution time because we can see that strlen(buf) is small.. right?
	// does the signed get cast to unsigned or the unsigned to signed for this comparison? Or is it undefined and that's the problem?
	if (bytesWritten < 0) {
		perror("Couldn't write to log file");
		return 1;
	}
	else if ((size_t) bytesWritten != strlen(buf)) {
		fprintf(stderr, "Couldn't write desired bytes to log file\n");
		return 1;
	}
	return 0;
}

/*
* Convert IP from addrstruct to string
* addrstr must be 16 bytes char array
* Does addrst[16] mean anything to the compiler or is it just same as char*
*/
int stringIP(char addrstr[16], in_addr_t ip)
{
	// from a 1521 sample code
	in_addr_t addrNum = ntohl(ip);
	snprintf (
		addrstr, 16, "%u.%u.%u.%u",
		addrNum >> 24 & 0xff, // why do you need to & on this one
		addrNum >> 16 & 0xff,
		addrNum >>  8 & 0xff,
		addrNum       & 0xff
	);
	return 0;
}

/*
* Drop uid and euid to goaluid, and test success
* exit() on failure
* gid??
*/
int dropPermissions(unsigned short goaluid) {
	printf("Dropping permissions after binding...\n");
	// !! TODO: also need to drop group privileges!
	setuid(goaluid);
	printf("uid after drop: %d %d\n", getuid(), geteuid());
	setuid(0);
	printf("uid fter setuid: %d %d\n", getuid(), geteuid());
	seteuid(0);
	printf("uid after seteuid: %d %d\n", getuid(), geteuid());
	/*if (getuid() != goaluid || geteuid() != goaluid) {
		exit(PERMISSIONSERROR);
	}*/
	// use the above version, this is just because it's easier to deal with when testing
	// maybe I should use a #ifdef TESTING or something
	if (getuid() == 0 || geteuid() == 0) {
		exit(PERMISSIONSERROR);	
	}
	return 0;
	// does this check introduce security problems by calling setuid and seteuid 0?
	// adding them to the binary
}

/*
* Create socket, bind to port, and start listening
* Exit()s on failure
*/
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

/*
* Read port number from string, check range, and write to port
* Return 0 for success, 1 for out of range
*/
int getPort(const char *string, unsigned short *port)
{
    long lport = strtol(string, NULL, 0);
    if (lport > MAXPORT || lport < 0) {
        fprintf(stderr, "Provided port number not in range. Legal port numbers 0..%d", MAXPORT);
        return 1;
    }
    *port = (unsigned short) lport;
    return 0;
}


// can generalise some of this and readCert?
int readDomain(char domain[DOMAINSIZE])
{
	int fd = open(DOMAINFILE, O_RDONLY);
    if (fd < 0) {
        perror("Can't open domain config file");
        return 1;
    }
    // obviously deal with size etc.
    int bytesRead = read(fd, domain, DOMAINSIZE-1);
    if (bytesRead <= 0) {
        perror("Can't read domain config file");
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
