all: server client

server: server.o
	gcc -Wall -o $@ $^

client: client.o
	gcc -Wall -o $@ $^

server.o: server.c
	gcc -c -Wall -librt -o $@ $^

client.o: client.c
	gcc -c -Wall -librt -o $@ $^

clear:
	-rm *.o server client
