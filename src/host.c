/* Header file */
#include "host.h"

int
create_host(struct host_t *host)
{
	socklen_t addr_size;
	char client_paddr[16];
	int r, reuse;

	memset(&host->hints, 0, sizeof host->hints);
	host->hints.ai_family = AF_INET;
	host->hints.ai_socktype = SOCK_STREAM;
	host->hints.ai_flags = AI_PASSIVE;

	if ((r = getaddrinfo(NULL, PORT, &host->hints, &host->res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
		return 1;
	}

	if ((host->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return 2;
	}

	reuse = 1;
	setsockopt(host->sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	if (bind(host->sockfd, host->res->ai_addr, host->res->ai_addrlen) == -1) {
		perror("bind");
		return 3;
	}

	if (listen(host->sockfd, 5) == -1) {
		perror("listen");
		return 4;
	}

	addr_size = sizeof host->client_addr;
	if ((host->sockfd = accept(host->sockfd, (struct sockaddr*) &host->client_addr, &addr_size)) == -1) {
		perror("accept");
		return 5;
	}

	if (inet_ntop(AF_INET, &host->client_addr.sin_addr, client_paddr, 15) == NULL) {
		perror("inet_ntop");
		return 6;
	}

	printf("Client %s connected\n", client_paddr);

	return 0;
}

void
free_host(struct host_t *host)
{
	close(host->host);
	close(host->sockfd);

	freeaddrinfo(host->res);
}

