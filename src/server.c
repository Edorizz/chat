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

int
main(int argc, char **argv)
{
	int sockfd, client, r;
	socklen_t addr_size;
	char client_paddr[16];
	struct addrinfo *res, hints;
	struct sockaddr_in client_addr;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((r = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		return 1;
	}

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return 2;
	}

	if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
		perror("bind");
		return 3;
	}

	if (listen(sockfd, 5) == -1) {
		perror("listen");
		return 4;
	}

	addr_size = sizeof client_addr;
	if ((client = accept(sockfd, (struct sockaddr*) &client_addr, &addr_size)) == -1) {
		perror("accept");
		return 5;
	}

	if (inet_ntop(AF_INET, &client_addr.sin_addr, client_paddr, 15) == NULL) {
		perror("inet_ntop");
		return 6;
	}

	printf("Client %s connected\n", client_paddr);

	if (send(client, "Hello!", 6, 0) == -1) {
		perror("send");
		return 6;
	}

	freeaddrinfo(res);

	return 0;
}

