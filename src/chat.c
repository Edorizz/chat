/* Ncurses */
#include <ncurses.h>

/* Chat */
#include "chat.h"

void
usage(const char *name)
{
	fprintf(stderr, "%s: [-c host]\n", name);
}

int
main(int argc, char **argv)
{
	struct host_t chat;
	struct pollfd poll_sock;
	char buf[BUF_SIZ], input_buf[BUF_SIZ], host_ip[64], *hostname;
	int i, client_type, c, quit, input_i, r, scroll, height, width;

	/* Parse command line arguments */
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
				usage(argv[0]);

				return 1;
			}
		}
	}

	/* Establish connection */
	if (client_type == CLIENT) {
		create_client((struct client_t *) &chat, hostname);
		fprintf(stderr, "Succesfully connected to %s\n", hostname);
	} else if (client_type == HOST) {
		create_host(&chat);
	} else {
		fprintf(stderr, "invalid client type\n");
		return 8;
	}

	/* DEBUG
	struct addrinfo *a;

	for (a = chat.res; a; a = a->ai_next) {
		memset(host_ip, 0, sizeof host_ip);
		inet_ntop(a->ai_family, &a->ai_addr, host_ip, sizeof host_ip - 1);

		printf("%s\n", host_ip);
	}
	*/

	/* Initialize ncurses */
	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	timeout(0);

	/* Get terminal dimensions */
	getmaxyx(stdscr, height, width);

	/* Initialize colors */
	use_default_colors();
	start_color();
	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_BLUE, -1);

	/* Setup socket polling */
	poll_sock.fd = chat.sockfd;
	poll_sock.events = POLLIN;

	/* Show connected ip on the top right */
	memset(host_ip, 0, sizeof host_ip);
	inet_ntop(chat.res->ai_family, &chat.res->ai_addr, host_ip, sizeof host_ip - 1);
	mvprintw(0, width - strlen(host_ip) - 2, "%s", host_ip);
	move(height - 1, 0);

	quit = input_i = 0;
	scroll = 1;

	while (!quit) {
		switch ((c = getch())) {
		case KEY_F(1):
			quit = 1;

			break;

		case '\n':
			input_buf[input_i] = '\0';
			if (send(chat.sockfd, input_buf, input_i, 0) == -1) {
				perror("send");
				return 6;
			}

			input_i = 0;

			attron(COLOR_PAIR(2));
			mvprintw(scroll, 0, "you:");
			attroff(COLOR_PAIR(2));
			printw(" %s", input_buf);
			++scroll;

			move(height - 1, 0);
			clrtoeol();

			break;

		case KEY_BACKSPACE:
			if (input_i > 0) {
				input_buf[--input_i] = '\0';

				move(height - 1, 0);
				clrtoeol();
				mvprintw(height - 1, 0, "%s", input_buf);
			}

			break;

		default:
			if (c != ERR && input_i < BUF_SIZ - 1) {
				input_buf[input_i++] = c;
				input_buf[input_i] = '\0';

				move(height - 1, 0);
				clrtoeol();
				mvprintw(height - 1, 0, "%s", input_buf);
			}

			break;
		}

		if (poll(&poll_sock, 1, 10) && (r = recv(chat.sockfd, (void*) buf, BUF_SIZ - 1, 0))) {
			if (r == -1) {
				perror("recv");
				exit(7);
			}
			
			buf[r] = '\0';
			attron(COLOR_PAIR(1));
			mvprintw(scroll, 0, "stranger:");
			attroff(COLOR_PAIR(1));
			printw(" %s", buf);
			move(height - 1, input_i);
			++scroll;

			refresh();
		}
	}

	/* Close connection and free memory */
	if (client_type == CLIENT) {
		free_client((struct client_t *) &chat);
	} else if (client_type == HOST) {
		free_host(&chat);
	}

	endwin();
	return 0;
}

