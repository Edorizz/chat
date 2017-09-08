/* Header file */
#include "client.h"

int
create_client(struct client *client, const char *hostname)
{
	memset(&client->hints, 0, sizeof client->hints);
	client->hints.ai_family = AF_INET;
	client->hints.ai_socktype = SOCK_STREAM;

	if ((client->r = getaddrinfo(hostname, PORT, &client->hints, &client->res)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(client->r));
		return 2;
	}

	if ((client->sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return 3;
	}

	if (connect(client->sockfd, client->res->ai_addr, client->res->ai_addrlen) == -1) {
		perror("connect");
		return 4;
	}

	return 0;
}

void
free_client(struct client *client)
{
	freeaddrinfo(client->res);
}

