#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

#define MAXPORT 65535 // !! get max short value #defined somewhere in C - MAXSHORT?
#define MAXATTEMPTS 3
#define MAXQUEUE 5 // !!! some of these should just be with server?

// ERRORS:
// 1 and 2 are standard error numbers
#define ARGSERROR 3
#define CREDENTIALSERROR 4
#define BUILDSOCKETERROR 5
#define CLOSESOCKETERROR 6
#define CLOCKERROR 7
#define MALLOCERROR 8
#define THREADERROR 9
#define SEMAPHOREERROR 10
#define SENDERROR 11
#define EVENTFDERROR 12
#define SIGHANDLERERROR 11
#define TIMERFDERROR 14
#define FILEERROR 15

#define PERMISSIONSERROR 16
#define FILENOTFOUND 17

#define SIGINTEXIT 20 // remove?
// enum??

#define MAXLENGTH 150 // name !!
#define MAXPATH 150

//#define MAXNUMARGS 2

#define TRUE 1
#define FALSE 0
typedef char bool;

typedef char byte;

#define CLKID CLOCK_BOOTTIME

// REMOVE THIS !!
//typedef struct in_addr IPType;

#endif //PROJECT_CONSTANTS_H

