CFLAGS=-O3 -Wall -W -pedantic

pong: pong.c
	$(CC) $(CFLAGS) -o pong pong.c

clean:
	$(RM) pong
