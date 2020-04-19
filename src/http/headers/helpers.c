#define _XOPEN_SOURCE
#define __USE_XOPEN
#include <stdlib.h>
#include <stdio.h>

#include "helpers.h"

#define DATEFORMAT "%a, %d %b %Y %H:%M:%S %Z"


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
	size_t bytesWritten = strftime(buf, size, DATEFORMAT, tm);
    if (bytesWritten <= 0) {
        fprintf(stderr, "strftime failed in getHTTPDate");
        return 1;
    }
    return 0;
}

int timetoHTTPDate(time_t time, char *buf, unsigned int size)
{
    struct tm *tm = gmtime(&time);
    if (tm == NULL) {
        perror("gmtime() failed");
        return 1;
    }
    size_t bytesWritten = strftime(buf, size, DATEFORMAT, tm);
    if (bytesWritten <= 0) {
        return 1;
    }
    return 0;
}

time_t HTTPDatetotime(const char *headersstr)
{
    struct tm tm;
    char *cur = strptime(headersstr, DATEFORMAT, &tm);
    if (cur == NULL) { return -1; }
    time_t t = mktime(&tm);
    if (t == -1) { perror("mktime() failed"); }
    return t;
}
