/* C Library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* Threads */
#include <pthread.h>

/* Custom */
#include "chat.h"

struct thread_data_t {
	char buf[BUF_SIZ];
	int sockfd, in_flags;
};

void*
thread_input(void *in)
{
	struct thread_data_t *data = (struct thread_data_t *) in;
	int r;

	while (!data->in_flags) {
		if ((r = recv(data->sockfd, (void*) data->buf, BUF_SIZ - 1, 0))) {
			if (r == -1) {
				perror("recv");
				exit(7);
			}
			
			data->buf[r] = '\0';
			printf("stranger: %s\n", data->buf);
		}
	}

	pthread_exit(0);
}

int
main(int argc, char **argv)
{
	int sockfd, client, r;
	socklen_t addr_size;
	char client_paddr[16], buf[256];
	struct addrinfo *res, hints;
	struct sockaddr_in client_addr;
	struct thread_data_t t_data;
	pthread_attr_t attr;
	pthread_t tid;

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

	memset(&t_data, 0, sizeof t_data);
	t_data.sockfd = client;

	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, thread_input, (void *) &t_data);
		
	for (;;) {
		while (fgets(buf, BUF_SIZ, stdin)) {
			if (send(client, buf, strlen(buf) - 1, 0) == -1) {
				perror("send");

				t_data.in_flags = 1;
				pthread_join(tid, NULL);
				
				return 6;
			}
		}
	}

	t_data.in_flags = 1;
	pthread_join(tid, NULL);

	freeaddrinfo(res);

	return 0;
}

