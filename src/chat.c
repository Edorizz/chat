/* C Library */
#include <stdio.h>
/* Chat */
#include "chat.h"

void
usage(const char *name)
{
	printf("%s: if this is still here it's because I forgot to remove it\n", name);
}

int
main(int argc, char **argv)
{
	struct host_t chat;
	char buf[BUF_SIZ], *hostname;
	int i, client_type;
	
	client_type = HOST;
	for (i = 1; i != argc; ++i) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'c':
				client_type = CLIENT;
				hostname = argv[i + 1];
				break;

			default:
				fprintf(stderr, "unknown agrument '%c'\n", argv[i][1]);
				return 1;

			}
		}
	}

	if (client_type == CLIENT) {
		create_client((struct client_t *) &chat, hostname);
		fprintf(stderr, "succesfully connected to %s\n", hostname);
	} else if (client_type == HOST) {
		create_host(&chat);
	} else {
		fprintf(stderr, "invalid client type\n");
		return 8;
	}

	create_thread(&chat.thread, chat.sockfd);

	while (fgets(buf, BUF_SIZ, stdin)) {
		if (send(chat.sockfd, buf, strlen(buf) - 1, 0) == -1) {
			perror("send");

			free_thread(&chat.thread);
			
			return 6;
		}
	}

	free_thread(&chat.thread);

	if (client_type == CLIENT) {
		free_client((struct client_t *) &chat);
	} else if (client_type == HOST) {
		free_host(&chat);
	}

	return 0;
}

