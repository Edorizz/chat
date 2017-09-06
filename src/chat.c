/* Ncurses */
#include <ncurses.h>

/* Chat */
#include "chat.h"

#define SCROLL_UP	0
#define SCROLL_DOWN	1

#define TEXT_COLOR_RED	1
#define TEXT_COLOR_BLUE	2

struct msg_prefix {
	char *mp_prefix;
	int mp_color;
};

const struct msg_prefix prefix[2] = { { "stranger", TEXT_COLOR_RED },
				      { "you", TEXT_COLOR_BLUE } };

/* Doest not contain the message data but it's used to
 * locate it in 'msg_log.ml_buf'
 */
struct msg {
	int m_off, m_len, m_send;
};

/* Keeps track of all messages in the current conversation */
struct msg_log {
	int ml_cnt, ml_ind, ml_buf_siz;
	int ml_scrl_off, ml_scrl_ind;
	char *ml_buf;
	struct msg *ml_log;
};

void
usage(const char *name)
{
	fprintf(stderr, "%s: [-c host]\n", name);
}

void
print_msg(const char *msg, int offset, int pf)
{
	if (offset) {
		printw("%s\n", msg + offset);
	} else {
		attron(COLOR_PAIR(prefix[pf].mp_color));
		printw("%s: ", prefix[pf].mp_prefix);
		attroff(COLOR_PAIR(prefix[pf].mp_color));
		printw("%s\n", msg);
	}

}

void
log_draw(const struct msg_log *log)
{
	int i;

	clear();
	move(1, 0);

	if (log->ml_scrl_ind < log->ml_ind) {
		/* Draw the message at the top of the screen with an offset if necessary */
		print_msg(log->ml_buf + log->ml_log[log->ml_scrl_ind].m_off,
			  log->ml_scrl_off,
			  log->ml_log[log->ml_scrl_ind].m_send);
		
		/* Draw all other messages normally */
		for (i = log->ml_scrl_ind + 1; i < log->ml_ind; ++i) {
			print_msg(log->ml_buf + log->ml_log[i].m_off, 0, log->ml_log[i].m_send);
		}
	}

	refresh();
}

void
log_append(struct msg_log *log, const char *msg, int from)
{
	void *tmp;
	int off, len, send;

	/* Increase size of messages array if necessary */
	if (log->ml_ind >= log->ml_cnt) {
		tmp = malloc(log->ml_cnt * 2 * sizeof(struct msg));
		memcpy(tmp, log->ml_log, log->ml_cnt * sizeof(struct msg));
		free(log->ml_log);

		log->ml_log = tmp;
		log->ml_cnt <<= 1;
	}

	/* Hold message attributes */
	off = log->ml_log[log->ml_ind - 1].m_off + log->ml_log[log->ml_ind - 1].m_len + 1;
	len = strlen(msg);
	send = from;

	/* Increase size of buffer holding the actual text if necessary */
	if (off + len + 1 >= log->ml_buf_siz) {
		tmp = malloc(log->ml_buf_siz * 2);
		memcpy(tmp, log->ml_buf, log->ml_buf_siz);
		free(log->ml_buf);

		log->ml_buf = tmp;
		log->ml_buf_siz <<= 1;
	}

	/* Append new message to the array */
	log->ml_log[log->ml_ind].m_off = off;
	log->ml_log[log->ml_ind].m_len = len;
	log->ml_log[log->ml_ind].m_send = send;
	++log->ml_ind;

	/* Copy message text right after the previous one ends, with a null-terminating
	 * character separating them */
	memcpy(log->ml_buf + off, msg, len + 1);
}

void
log_scroll(struct msg_log *log, int dir, int scr_width)
{
	struct msg *m;
	int full_lines;

	if (dir == SCROLL_UP) {
		if (log->ml_scrl_off > 0) {
			log->ml_scrl_off -= scr_width;

			if (log->ml_scrl_off < 0) {
				log->ml_scrl_off = 0;
			}

		} else if (log->ml_scrl_off == 0 && log->ml_scrl_ind > 0) {
			m = &log->ml_log[--log->ml_scrl_ind];

			full_lines = (m->m_len + strlen(prefix[m->m_send].mp_prefix) + 2) / scr_width;

			if (full_lines > 0) {
				log->ml_scrl_off = scr_width - (strlen(prefix[m->m_send].mp_prefix) + 2) +
					(full_lines - 1) * scr_width;
			} else {
				log->ml_scrl_off = 0;
			}
		}

	} else if (dir == SCROLL_DOWN) {
		m = &log->ml_log[log->ml_scrl_ind];

		if (log->ml_scrl_off > 0) {
			log->ml_scrl_off += scr_width;
		} else if (log->ml_scrl_off == 0 && log->ml_scrl_ind < log->ml_ind - 1) {
			log->ml_scrl_off = scr_width - (strlen(prefix[m->m_send].mp_prefix) + 2);
		}

		if (log->ml_scrl_off >= m->m_len) {
			log->ml_scrl_off = 0;
			++log->ml_scrl_ind;
		}
	}
}

int
main(int argc, char **argv)
{
	struct host_t chat;
	struct pollfd poll_sock;
	struct msg_log log;
	char buf[BUF_SIZ], input_buf[BUF_SIZ], host_ip[64], *hostname;
	int i, client_type, c, quit, input_i, r, height, width;

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
	init_pair(TEXT_COLOR_RED, COLOR_RED, -1);
	init_pair(TEXT_COLOR_BLUE, COLOR_BLUE, -1);

	/* Setup socket polling */
	poll_sock.fd = chat.sockfd;
	poll_sock.events = POLLIN;

	/* Show connected ip on the top right */
	memset(host_ip, 0, sizeof host_ip);
	inet_ntop(chat.res->ai_family, &chat.res->ai_addr, host_ip, sizeof host_ip - 1);
	mvprintw(0, width - strlen(host_ip) - 2, "%s", host_ip);
	move(height - 1, 0);

	/* Setup message log */
	memset(&log, 0, sizeof log);
	log.ml_cnt = 256;
	log.ml_buf_siz = 256 * 256;

	log.ml_buf = malloc(log.ml_buf_siz);
	log.ml_log = malloc(log.ml_cnt * sizeof(struct msg));

	quit = input_i = 0;

	while (!quit) {
		switch ((c = getch())) {
		case KEY_F(1):
			quit = 1;

			break;

		case KEY_F(2):
			log_scroll(&log, SCROLL_DOWN, width);
			log_draw(&log);

			break;

		case KEY_F(3):
			log_scroll(&log, SCROLL_UP, width);
			log_draw(&log);

			break;
			
		case '\n':
			input_buf[input_i] = '\0';
			if (send(chat.sockfd, input_buf, input_i, 0) == -1) {
				perror("send");
				return 6;
			}

			input_i = 0;

			log_append(&log, input_buf, 1);
			log_draw(&log);

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

			log_append(&log, buf, 0);
			log_draw(&log);
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

