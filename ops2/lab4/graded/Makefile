compile: server

server: server.o
	gcc -Wall -o $@ $^

server.o: server.c
	gcc -c -Wall -o $@ $^

clear:
	-rm server *.o
