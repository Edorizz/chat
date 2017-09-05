.POSIX:
.SUFFIXES:

CC := gcc
CFLAGS := -Wall -pedantic -c
LDLIBS := -pthread -lncurses
NAME := chat
MKDIR := mkdir -p
RM := rm -f

all: $(NAME) obj

obj:
	$(MKDIR) obj/

$(NAME): obj/chat.o obj/client.o obj/host.o obj/threads.o
	$(CC) $(LDFLAGS) $(LDLIBS) obj/chat.o obj/client.o obj/host.o obj/threads.o -o $(NAME)

obj/chat.o: src/chat.c src/chat.h
	$(CC) $(CFLAGS) src/chat.c -o obj/chat.o

obj/client.o: src/client.c src/chat.h src/threads.h
	$(CC) $(CFLAGS) src/client.c -o obj/client.o

obj/host.o: src/host.c src/chat.h src/threads.h
	$(CC) $(CFLAGS) src/host.c -o obj/host.o

obj/threads.o: src/threads.c src/threads.h
	$(CC) $(CFLAGS) src/threads.c -o obj/threads.o

.PHONY:
clean:
	$(RM) obj/*.o $(NAME)
