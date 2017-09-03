.POSIX:
.SUFFIXES:

CC := gcc
CFLAGS := -Wall -pedantic -c
LDLIBS := -pthread
MKDIR := mkdir -p
RM := rm -f

all: client server obj

obj:
	$(MKDIR) obj/

client: obj/client.o
	$(CC) $(LDFLAGS) $(LDLIBS) obj/client.o -o client

server: obj/server.o
	$(CC) $(LDFLAGS) $(LDLIBS) obj/server.o -o server

obj/client.o: src/client.c
	$(CC) $(CFLAGS) src/client.c -o obj/client.o

obj/server.o: src/server.c
	$(CC) $(CFLAGS) src/server.c -o obj/server.o

.PHONY:
clean:
	$(RM) obj/*.o client server
