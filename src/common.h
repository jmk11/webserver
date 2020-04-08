void printSource(struct sockaddr_in addrStruct, const char *prefix);
int dropPermissions(unsigned short goaluid);
int buildSocket(unsigned short port);
int logSource(int logfd, struct sockaddr_in addrStruct);