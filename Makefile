CFLAGS=-O3 -Wall -W -pedantic

.PHONY: all clean

all: pong ping

pong: pong.c
	$(CC) $(CFLAGS) -o pong pong.c

ping: ping.c
	$(CC) $(CFLAGS) -o ping ping.c

clean:
	$(RM) pong
	$(RM) ping
