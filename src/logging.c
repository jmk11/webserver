#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "logging.h"
#include "common.h"
#include "http/helpers.h"
// #include "http/request/requestheaders.h"

/*
* Log time, source IP and port, resource request, user agent to given file descriptor
*/
int logRequest(int logfd, struct sockaddr_in addrStruct, const requestHeaders *requestHeaders)
{
	unsigned int bufsize = 300;
	char buf[bufsize];
	char *cur;
	unsigned int remsize;
	
	// write time to buf
	int bytesWritten = getHTTPDate(buf, bufsize);
	if (bytesWritten <= 0) {
		fprintf(stderr, "Can't get text date for logRequest\n");
		// do something
	}
	cur = buf + bytesWritten;
	remsize = bufsize - bytesWritten;
	
	//  get IP as string
	char addrstr[16];
	stringIP(addrstr, addrStruct.sin_addr.s_addr);
	// write IP+port, resource, user agent to buf
	bytesWritten = snprintf(cur, remsize, ": %s:%hu\n\t%s\n\t%s\n", addrstr, addrStruct.sin_port, requestHeaders->resource, requestHeaders->UserAgent);
	// todo: check failure
	
	// write buf to file descriptor
	bytesWritten = write(logfd, buf, strlen(buf));
	if (bytesWritten < 0) {
		perror("Couldn't write to log file");
		return 1;
	}
	else if ((size_t) bytesWritten != strlen(buf)) {
		fprintf(stderr, "Couldn't write desired bytes to log file\n");
		return 1;
	}
	return 0;
	
	// cur += bytesWritten
	// remsize -= bytesWritten
	// strlcat3(char *dstStart, char **dstCur, const char *src, unsigned int maxSize)
	// strlcat3(buf, &cur, " %s:%hu\n", 
	
	// resource request
	// bytesWritten = snprintf(cur, remsize, "\t %s", requestHeaders.resource);
}
