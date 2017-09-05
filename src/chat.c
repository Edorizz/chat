/* C Library */
#include <stdio.h>

/* Ncurses */
#include <ncurses.h>

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
	struct pollfd poll_sock;
	char buf[BUF_SIZ], input_buf[BUF_SIZ], *hostname;
	int i, client_type, c, quit, input_i, r, scroll, height;

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

	/* Establish connection */
	if (client_type == CLIENT) {
		create_client((struct client_t *) &chat, hostname);
		fprintf(stderr, "succesfully connected to %s\n", hostname);
	} else if (client_type == HOST) {
		create_host(&chat);
	} else {
		fprintf(stderr, "invalid client type\n");
		return 8;
	}

	/* Initialize ncurses */
	initscr();
	keypad(stdscr, TRUE);
	cbreak();
	noecho();
	timeout(0);

	/* Get terminal dimensions */
	height = getmaxy(stdscr);

	/* Initialize colors */
	use_default_colors();
	start_color();
	init_pair(1, COLOR_RED, -1);
	init_pair(2, COLOR_BLUE, -1);

	poll_sock.fd = chat.sockfd;
	poll_sock.events = POLLIN;

	quit = input_i = scroll = 0;
	move(height - 1, 0);

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

