#ifndef THREADS_H
#define THREADS_H

/* Threads */
#include <pthread.h>

#define BUF_SIZ	256

struct thread_data_t {
	char buf[BUF_SIZ];
	int sockfd, in_flags;
};

struct thread_t {
	struct thread_data_t t_data;
	pthread_attr_t attr;
	pthread_t tid;
};

void *t_recv(void *in);
int  create_thread(struct thread_t *thread, int sockfd);
void free_thread(struct thread_t *thread);

#endif /* THREADS_H */

