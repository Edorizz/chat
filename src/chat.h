#ifndef CHAT_H
#define CHAT_H

#define PORT	"1234"
#define BUF_SIZ	256

/* Client types */
#define HOST	0
#define CLIENT	1

/* C Library */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Sockets */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* Chat */
#include "client.h"
#include "host.h"

#endif /* CHAT_H */

