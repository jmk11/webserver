#ifndef PROJECT_CONSTANTS_H
#define PROJECT_CONSTANTS_H

//#define MAXPORT 65535 // !! get max short value #defined somewhere in C - MAXSHORT?
//#define MAXATTEMPTS 3
//#define MAXQUEUE 5 // !!! some of these should just be with server?

#define PERMISSIONSERROR 16
#define FILENOTFOUND 17
#define SIGINTEXIT 20 // remove?

#define MAXLENGTH 150 // name !!
#define MAXPATH 150

//#define MAXNUMARGS 2

/*
#define TRUE 1
#define FALSE 0
typedef char bool;
*/

/*
enum bool {
    FALSE,
    TRUE
};
typedef enum bool bool;
*/
// bool a = 182340932;
// this assignment doesn't give warning - it lets you assign anything to an enum.
// What is the point of enum???

typedef char byte;

// REMOVE THIS !!
//typedef struct in_addr IPType;

#endif //PROJECT_CONSTANTS_H

