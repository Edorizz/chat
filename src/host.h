#ifndef HOST_H
#define HOST_H

#include "chat.h"
#include "threads.h"

struct host_t {
	/* [Socket] */
	int sockfd, r;
	struct addrinfo *res, hints;
	/* [Thread] */
	struct thread_t thread;

	/* [Extra stuff needed by the host] */
	int host;
	struct sockaddr_in client_addr;
};

int  create_host(struct host_t *host);
void free_host(struct host_t *host);

#endif /* HOST_H */

