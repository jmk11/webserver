#ifndef STATUSCODES_H
#define STATUSCODES_H

#define STATUS200 STATUS_OK
#define STATUS304 STATUS_NOTMODIFIED
#define STATUS400 STATUS_BADREQUEST
#define STATUS404 STATUS_NOTFOUND
#define STATUS405 STATUS_METHODNOTALLOWED
#define STATUS408 STATUS_REQUESTTIMEOUT
#define STATUS414 STATUS_URITOOLONG
#define STATUS431 STATUS_REQUESTHEADERFIELDSTOOLARGE
#define STATUS500 STATUS_INTERNALSERVERERROR
#define STATUS501 STATUS_NOTIMPLEMENTED
#define STATUS505 STATUS_HTTPVERSIONNOTSUPPORTED

#define STATUS_OK                           "200 OK"
#define STATUS_NOTMODIFIED                  "304 Not Modified"
#define STATUS_BADREQUEST                   "400 Bad Request"
#define STATUS_NOTFOUND                     "404 Not Found"
#define STATUS_METHODNOTALLOWED             "405 Method Not Allowed"
#define STATUS_REQUESTTIMEOUT               "408 Request Timeout"
#define STATUS_URITOOLONG                   "414 URI Too Long"
#define STATUS_REQUESTHEADERFIELDSTOOLARGE  "431 Request Header Fields Too Large"
#define STATUS_INTERNALSERVERERROR          "500 Internal Server Error"
#define STATUS_NOTIMPLEMENTED               "501 Not Implemented"
#define STATUS_HTTPVERSIONNOTSUPPORTED      "505 HTTP Version Not Supported"

// Obv it makes more sense to use the names as labels but I like the numbers

#endif /* STATUSCODES_H */
