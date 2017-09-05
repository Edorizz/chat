# chat
'chat' is a small program used to chat with your friends using the power of the interwebs, this is mainly a learning excersise to learn my way around sockets
so it souldn't be taken super seriously, but any type of feedback is welcome.

## Building
This depends on Ncurses for terminal which can be easily installed using your preferred package manager (if needed).

To compile just run:
```
make
```
And to run:
```
./chat [-options]
```

## Command line options
```
usage: ./chat [-c host]
```

-c:		enable client mode, next argument should where to connect to (host ip).
