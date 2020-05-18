//#define __USE_MISC

// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <sys/types.h>
// #include <string.h>
// #include <stdlib.h>
// #include <assert.h>
// #include <sys/stat.h>
// #include <limits.h>
// #include <linux/limits.h>
// #include <string.h>
// #include <linux/stat.h>
// #include <stdio.h>
#include <fcntl.h>
#include <unistd.h>


#include "uid.h"
#include "common.h"
#include "connection.h"
#include "wrappers/wrappers.h" // only needs this for error codes, this isn't right
#include "ssl/ssl.h"
#include "http/response/contenttype.h"
#include "http/request/headerfns.h"

#define LOGFILE "logs/serverlog.txt"
#define SSLCONFIGLOCATION "config/certs.txt"

int main(int argc, char **argv)
{
	bool fileLogging = TRUE;

	// initialise ssl
	SSL_CTX *ctx = setupssl(SSLCONFIGLOCATION);
	if (ctx == NULL) {
		fprintf(stderr, "SSL setup failure. Exiting.\n");
		return 1;
	}

	printf("uid at start: %d\n", getuid());
	printf("euid at start: %d\n", geteuid());
	
	// get port
	unsigned short port;
	if (argc == 2) {
		int res = getPort(argv[1], &port);
		if (res != 0) {
			fprintf(stderr, "Exiting.\n");
			return 1;
		}
	}
	else {
		port = 443;
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

	// initialise content type and header function hash tables
	int res = buildContentTypeHT(); // get rid of this, main shouldn't know about this maybe
	if (res != 0) {
		fprintf(stderr, "Couldn't build content type hash table.\nExiting.\n");
		return 1;
	}
	res = buildHeaderFnsHT();
	if (res != 0) {
		fprintf(stderr, "Couldn't build header functions hash table.\nExiting.\n");
		return 1;
	}

	char domain[DOMAINSIZE];
	res = readDomain(domain);
	if (res != 0) {
		fprintf(stderr, "Can't read domain name from config file. Exiting\n");
		return 1;
	}

	// open log file
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
			perror("accept clientfd < 0");
		}
		else {
			printSource(clientAddr, NULL);
			if (fileLogging) { logSource(logfd, clientAddr); }
			// spawn new thread
			handleConnection(clientfd, ctx, logfd, clientAddr, domain);
			close(clientfd);
			// what do if can't close client socket?
		}
	}

	destroyContentTypeHT();
	destroyHeaderFnsHT();
	Close(logfd);
	Close(serverfd);
	cleanssl(ctx);
	return 0;	
}
