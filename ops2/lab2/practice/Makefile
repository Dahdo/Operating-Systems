mq_practice: mq_practice.o
	gcc -Wall -o $@ $^

mq_practice.o: mq_practice.c
	gcc -c -Wall -librt -o $@ $^

clear:
	-rm mq_practice mq_practice.o
