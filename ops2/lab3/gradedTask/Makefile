CC=gcc
CFLAGS= -std=gnu99 -Wall -fsanitize=address,undefined

compile: server.c client.c
	$(CC) $(CFLAGS) -o client client.c
	$(CC) $(CFLAGS) -o server server.c


clear:
	-rm -rf server client