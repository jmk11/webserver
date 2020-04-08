//#define __USE_MISC

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
//#include <limits.h>
#include <linux/limits.h>
#include <string.h>
//#include <fcntl.h>
//#include <linux/stat.h>

//#include "helpers.h"
#include "helpers.h"
#include "constants.h"
#include "uid.h"
//#include "headers.h"
//#include "custom.h"
#include "ssl/ssl.h"
#include "common.h"
#include "contenttype.h"
#include "wrappers.h" // only needs this for error codes, this isn't right
#include "connection.h"

#define LOGFILE "logs/serverlog.txt"
#define SSLCONFIGLOCATION "config/certs.txt"
bool fileLogging = TRUE;

int main(int argc, char **argv)
{
	SSL_CTX *ctx = setupssl(SSLCONFIGLOCATION);
	if (ctx == NULL) {
		fprintf(stderr, "SSL setup failure. Exiting.\n");
		return 1;
	}

	printf("uid at start: %d\n", getuid());
	
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

	int logfd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0660);
	if (logfd < 0) {
		perror("Cannot open log file for writing");
		fprintf(stderr, "Continuing execution without logging to file..");
		fileLogging = FALSE;
	}

	int res = buildContentTypeHT();
	if (res != 0) {
		fprintf(stderr, "Couldn't build content type hash table.\nExiting.\n");
		return 1;
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
			handleConnection(clientfd, ctx);
			close(clientfd);
			// what do if can't close client socket?
		}
	}
	
	Close(logfd);
	Close(serverfd);
	cleanssl(ctx);
	return 0;	
}
