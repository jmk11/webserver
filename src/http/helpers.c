#define _XOPEN_SOURCE // for strptime
//#define __USE_XOPEN
#define _DEFAULT_SOURCE // for timegm // not sure about this
// #include <stdlib.h>
#include <stdio.h>

#include "helpers.h"

#define DATEFORMAT "%a, %d %b %Y %H:%M:%S %Z"


/*
* Convert current time into HTTP date format
* And place in buf
* Returns number of bytes written, or -1 on failure
*/
int getHTTPDate(char *buf, unsigned int size)
{
    time_t t = time(NULL);
    if (t == -1) {
        perror("time() failed");
        return -1;
    }
	struct tm *tm = gmtime(&t);
    if (tm == NULL) {
        perror("gmtime() failed");
        return -1;
    }
	size_t bytesWritten = strftime(buf, size, DATEFORMAT, tm);
    if (bytesWritten <= 0) {
        fprintf(stderr, "strftime failed in getHTTPDate");
    }
    return bytesWritten;
}

/*
* Converts given time to HTTP date format
* And places in buf
* Returns 0 on success, 1 on failure
*/
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

/*
* Converts HTTP date string to time_t time
* Returns -1 on failure
*/
time_t HTTPDatetotime(const char *httpdate)
{
    struct tm tm;
    char *cur = strptime(httpdate, DATEFORMAT, &tm);
    if (cur == NULL) { return -1; }
    time_t t = timegm(&tm);
    if (t == -1) { perror("mktime() failed"); }
    return t;

    //tm.tm_isdst = 0;
    //tm.tm_gmtoff = 0;
    //tm.tm_zone = "GMT";
    //tm.__tm_gmtoff = 0;
    //tm.__tm_zone = "GMT";
    // strptime() does not support %Z. I do not think this manual setting is a proper solution.
    // and I'm not sure if the isdst value changes depending on time of year
    //time_t t = mktime(&tm);
}
