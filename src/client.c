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
t_recv(void *in)
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
	struct thread_data_t t_data;
	pthread_attr_t attr;
	pthread_t tid;
	char buf[BUF_SIZ];
	
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

	fprintf(stderr, "Succesfully connected to %s\n", argv[1]);

	memset(&t_data, 0, sizeof t_data);
	t_data.sockfd = sockfd;

	pthread_attr_init(&attr);
	pthread_create(&tid, &attr, t_recv, (void *) &t_data);

	for (;;) {
		while (fgets(buf, BUF_SIZ, stdin)) {
			if (send(sockfd, buf, strlen(buf) - 1, 0) == -1) {
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

