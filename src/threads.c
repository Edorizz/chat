/* Header file */
#include "threads.h"

/* C Library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Sockets */
#include <sys/types.h>
#include <sys/socket.h>

void*
t_recv(void *in)
{
	struct thread_data_t *data = (struct thread_data_t *) in;
	int r;

	while (!data->in_flags) {
	}

	pthread_exit(0);
}

int
create_thread(struct thread_t *thread, int sockfd)
{
	memset(&thread->t_data, 0, sizeof thread->t_data);
	thread->t_data.sockfd = sockfd;

	pthread_attr_init(&thread->attr);
	pthread_create(&thread->tid, &thread->attr, t_recv, (void *) &thread->t_data);

	return 0;
}

void
free_thread(struct thread_t *thread)
{
	thread->t_data.in_flags = 1;
	pthread_join(thread->tid, NULL);
}

