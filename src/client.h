#ifndef CLIENT_H
#define CLIENT_H

#include "chat.h"

struct client_t {
	int sockfd, r;
	struct addrinfo *res, hints;
};

int  create_client(struct client_t *chat, const char *hostname);
void free_client(struct client_t *client);

#endif /* CLIENT_H */

