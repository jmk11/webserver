#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>

int getPort(const char *string, unsigned short *port);

void printSource(struct sockaddr_in addrStruct, const char *prefix);
int dropPermissions(unsigned short goaluid);
int buildSocket(unsigned short port);
int logSource(int logfd, struct sockaddr_in addrStruct);


#endif /* COMMON_H */
