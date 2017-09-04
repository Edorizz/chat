#ifndef CLIENT_H
#define CLIENT_H

#include "chat.h"
#include "threads.h"

struct client_t {
	/* [Socket] */
	int sockfd, r;
	struct addrinfo *res, hints;
	/* [Thread] */
	struct thread_t thread;
};

int  create_client(struct client_t *chat, const char *hostname);
void free_client(struct client_t *client);

#endif /* CLIENT_H */

