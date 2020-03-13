CFLAGS=-O3 -Wall -W -pedantic

.PHONY: all clean

all: pong ping pongudp

pong: pong.c
	$(CC) $(CFLAGS) -o pong pong.c

ping: ping.c
	$(CC) $(CFLAGS) -o ping ping.c

pongudp: pongudp.c
	$(CC) $(CFLAGS) -o pongudp pongudp.c

clean:
	$(RM) pong
	$(RM) ping
