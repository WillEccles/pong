pong: pong.c
	$(CC) $(CFLAGS) -o pong pong.c

clean:
	$(RM) pong
