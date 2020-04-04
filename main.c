#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <assert.h>


int buildSocket(unsigned short port);

int main(int argc, char **argv)
{
	printf("Start: %d\n", getuid());
	unsigned short port;
	if (argc == 2) {
		port = atoi(argv[1]);
	} else {
		port = 80;
	}
	int serversockfd = buildSocket(port);
	setuid(1000);
	printf("After drop: %d %d\n", getuid(), geteuid());
	setuid(0);
	printf("After setuid: %d %d\n", getuid(), geteuid());
	seteuid(0);
	printf("After seteuid: %d %d\n", getuid(), geteuid());
	assert(getuid() != 0);

	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientsockfd;
	int res;
	while (1) {
		clientsockfd = accept(serversockfd, (struct sockaddr *) &clientAddr, &clientAddrLen);
		char response[BUFSIZ] = "HTTP/1.1 200 OK\r\n"
								"Content-Type: text/html; charset=utf-8\r\n\r\n"
								"<html><title>Justin</title><body><h3><center>Under Construction</br></br></br></br></br>Coming Christmas 2002!</h3></center></body></html>";	
		res = send(clientsockfd, response, strlen(response), 0);
		if (res == -1) {
			perror("send");
			return 1;
		}
		close(clientsockfd);
	}
	close(serversockfd);

	return 0;	
}

int buildSocket(unsigned short port) {
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	(serverAddr).sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	int res = bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	if (res != 0) {
		perror("bind");
		exit(1);
	}
	listen(sockfd, 5);
	return sockfd;
}
