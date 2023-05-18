compile: client2 server2

client2: client2.o
	gcc -Wall -o $@ $^

client2.o: client2.c
	gcc -c -Wall -o $@ $^

server2: server2.o
	gcc -Wall -o $@ $^

server2.o: server2.c
	gcc -c -Wall -o $@ $^

clear:
	-rm server2 client2 *.o
