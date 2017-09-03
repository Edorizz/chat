/* C Library */
#include <stdio.h>
#include <string.h>

/* Sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* Custom */
#include "chat.h"

void
usage(const char *name)
{
	printf("%s: server\n", name);
}

int
main(int argc, char **argv)
{
	int sockfd, r;
	struct addrinfo *res, hints;
	char buf[256];
	
	if (argc == 1) {
		usage(argv[0]);
		return 1;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	if ((r = getaddrinfo(argv[1], PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		return 2;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return 3;
	}

	if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("connect");
		return 4;
	}

	r = recv(sockfd, (void*) buf, 255, 0);
	buf[r] = '\0';

	printf("Server said: \"%s\"\n", buf);

	freeaddrinfo(res);

	return 0;
}

