prog1: prog1.o
	gcc -Wall -fsanitize=address,undefined -o prog1 prog1.o

prog1.o: prog1.c
	gcc -Wall -fsanitize=address,undefined -o prog1.o -c prog1.c

.PHONY:
	-rm -f prog1 prog1.o
