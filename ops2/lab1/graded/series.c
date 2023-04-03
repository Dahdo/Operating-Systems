#define _GNU_SOURCE
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void usage(char *name)
{
	fprintf(stderr, "USAGE: %s fifo_file modifier siries...\n", name);
	exit(EXIT_FAILURE);
}

void read_from_fifo(int fifo)
{
	int count;
    count = 0;
    unsigned char c;
	do {
		count = TEMP_FAILURE_RETRY(read(fifo, &c, 1));
		if (count < 0)
			ERR("read");
		    printf("read %d\n", c);
	} while (count > 0);
}

void child_work(int fifoWrite, int parentFifo, int* numbers, int modifier, int index) {
    int number = numbers[index];
    srand(getpid());
    int random = -1 * modifier + rand() % (2*modifier + 1);
    printf("%d\n", number + random); //for stage 1

    int count = 0;
    unsigned char c = (unsigned char)number + random;
    if ((count = TEMP_FAILURE_RETRY(write(fifoWrite, &c, 1))) < 0)
			ERR("read");

}

void create_children_and_fifos(int elementCount, int parentFifo, int modifier, int* childFifos, int* numbers, char* file)
{
    int n = 0;
    int fifo;
	while ( n < elementCount) {
	    if (mkfifo(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) < 0)
	    if (errno != EEXIST)
		    ERR("create fifo");

	    if ((fifo = open(file, O_WRONLY|O_NONBLOCK)) < 0)
		    ERR("open");

		switch (fork()) {
		case 0:
            int index = n;
			child_work(fifo, parentFifo, numbers, modifier, index);
			exit(EXIT_SUCCESS);
		case -1:
			ERR("Fork:");
		}
		childFifos[n++] = fifo;
	}
}


int main(int argc, char **argv)
{
	int fifo, elementCount, modifier, *numbers, *childFifos;
	if (argc < 4 || argc > 13)
		usage(argv[0]);

    elementCount = argc - 3;
    numbers = malloc(sizeof(int) * elementCount);
    childFifos = malloc(sizeof(int) * elementCount);
    modifier = atoi(argv[2]);

    for(int i = 0; i < elementCount; i++)  
    numbers[i] = atoi(argv[i + 3]);

	if (mkfifo(argv[1], S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP) < 0)
		if (errno != EEXIST)
			ERR("create fifo");

	if ((fifo = open(argv[1], O_RDONLY|O_NONBLOCK)) < 0)
		ERR("open");
    
    create_children_and_fifos(elementCount, fifo, modifier, childFifos, numbers, argv[1]);
	sleep(3);
	read_from_fifo(fifo);
	if (close(fifo) < 0)
		ERR("close fifo:");
	if (unlink(argv[1]) < 0)
		ERR("remove fifo:");
	return EXIT_SUCCESS;
}