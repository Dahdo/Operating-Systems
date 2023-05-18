compile: client server

client: client.o
	gcc -Wall -o $@ $^

client.o: client.c
	gcc -c -Wall -o $@ $^

server: server.o
	gcc -Wall -o $@ $^

server.o: server.c
	gcc -c -Wall -o $@ $^

clear:
	-rm server client *.o
