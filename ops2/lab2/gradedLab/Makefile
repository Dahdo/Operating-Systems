all: host guest

host: host.o
	gcc -Wall -o $@ $^

host.o: host.c
	gcc -c -Wall -librt -o $@ $^

guest: guest.o
	gcc -Wall -o $@ $^

guest.o: guest.c
	gcc -c -Wall -librt -o $@ $^

clear:
	-rm *.o host guest
