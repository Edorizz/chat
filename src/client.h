#ifndef CLIENT_H
#define CLIENT_H

#include "chat.h"

struct client {
	int sockfd, r;
	struct addrinfo *res, hints;
};

int  create_client(struct client *chat, const char *hostname);
void free_client(struct client *client);

#endif /* CLIENT_H */

