#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>

#define DOMAINSIZE 100
#define DOMAINFILE "config/domain.txt"

int getPort(const char *string, unsigned short *port);

void printSource(struct sockaddr_in addrStruct, const char *prefix);
int dropPermissions(unsigned short goaluid);
int buildSocket(unsigned short port);
int logSource(int logfd, struct sockaddr_in addrStruct);
int stringIP(char addrstr[16], in_addr_t ip);
int readDomain(char domain[DOMAINSIZE]);



#endif /* COMMON_H */
