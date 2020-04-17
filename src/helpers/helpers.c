#include <stdio.h>
//#include <string.h>
#include <time.h>
//#include <error.h>
//#include <signal.h>
//#include <unistd.h>
#include <stdlib.h>
//#include <assert.h>

#include "helpers.h"

// not sure this fits with what helpers.c has become
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

int getHTTPDate(char *buf, unsigned int size)
{
    time_t t = time(NULL);
    if (t == -1) {
        perror("time() failed");
        return 1;
    }
	struct tm *tm = gmtime(&t);
    if (tm == NULL) {
        perror("gmtime() failed");
        return 1;
    }
	size_t bytesWritten = strftime(buf, size, "%a, %d %b %Y %H:%M:%S %Z", tm);
    if (bytesWritten <= 0) {
        return 1;
    }
    return 0;
}
