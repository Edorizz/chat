.POSIX:
.SUFFIXES:

CC := gcc
CFLAGS := -Wall -pedantic -c
LDLIBS := -lncurses
NAME := chat
MKDIR := mkdir -p
RM := rm -f

all: $(NAME) obj

obj:
	$(MKDIR) obj/

$(NAME): obj/chat.o obj/client.o obj/host.o
	$(CC) $(LDFLAGS) $(LDLIBS) obj/chat.o obj/client.o obj/host.o -o $(NAME)

obj/chat.o: src/chat.c src/chat.h
	$(CC) $(CFLAGS) src/chat.c -o obj/chat.o

obj/client.o: src/client.c src/chat.h
	$(CC) $(CFLAGS) src/client.c -o obj/client.o

obj/host.o: src/host.c src/chat.h
	$(CC) $(CFLAGS) src/host.c -o obj/host.o

.PHONY:
clean:
	$(RM) obj/*.o $(NAME)
