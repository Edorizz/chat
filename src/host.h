#ifndef HOST_H
#define HOST_H

#include "chat.h"

struct host {
	int sockfd, r;
	struct addrinfo *res, hints;

	/* [Extra stuff needed by the host] */
	int host;
	struct sockaddr_in client_addr;
};

int  create_host(struct host *host);
void free_host(struct host *host);

#endif /* HOST_H */

