#define _GNU_SOURCE
#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define BLOCKS 3

volatile sig_atomic_t finished;

void error(char *);
void usage(char *);
// void siginthandler(int);
// void sethandler(void (*)(int), int);
off_t getfilelength(int);
// void fillaiostructs(struct aiocb *, char **, int, int);
// void suspend(struct aiocb *);
// void readdata(struct aiocb *, off_t);
// void writedata(struct aiocb *, off_t);
// void syncdata(struct aiocb *);

void erro(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void usage(char *progname)
{
    fprintf(stderr, "%s workfile blocksize\n", progname);
    fprintf(stderr, "workfile - path to the file to work on\n");
    fprintf(stderr, "n - number of blocks\n");
    fprintf(stderr, "k - number of iterations\n");
    exit(EXIT_FAILURE);
}

off_t getfilelength(int fd)
{
    struct stat buf;
    if (fstat(fd, &buf) == -1)
        error("Cannot fstat file");
    return buf.st_size;
}

void fillaiostructs(struct aiocb *aiocbs, char **buffer, int fd, int blocksize)
{
    int i;
    for (i = 0; i < BLOCKS; i++)
    {
        memset(&aiocbs[i], 0, sizeof(struct aiocb));
        aiocbs[i].aio_fildes = fd;
        aiocbs[i].aio_offset = 0;
        aiocbs[i].aio_nbytes = blocksize;
        aiocbs[i].aio_buf = (void *)buffer[i];
        aiocbs[i].aio_sigevent.sigev_notify = SIGEV_NONE;
    }
}

int main(int argc, char *argv[])
{
    char *filename, *buffer[BLOCKS];
    int fd, n, blocksize, remainingsize;
    struct aiocb aiocbs[3];
    if (argc != 3)
        usage(argv[0]);
    filename = argv[1];
    n = atoi(argv[2]);
    if (n < 2)
        return EXIT_SUCCESS;
    finished = 1;

    if ((fd = TEMP_FAILURE_RETRY(open(filename, O_RDWR))) == -1)
        error("Cannot open file");

    blocksize = (getfilelength(fd) - 1) / n;
    remainingsize = (getfilelength(fd) - 1) % n;
    int sizeCount = 0;
    fprintf(stdout, "Blocksize: %ld\n", getfilelength(fd));
    for (int i = 0; i <= n; i++)
    {
        fprintf(stdout, "Blocksize: %d\n", sizeCount);
        sizeCount += blocksize;
    }
    fprintf(stdout, "Remaining part: %d\n", remainingsize);

    if (blocksize > 0)
    {
        for (int i = 0; i < BLOCKS; i++)
            if ((buffer[i] = (char *)calloc(blocksize, sizeof(char))) == NULL)
                error("Cannot allocate memory");
        void fillaiostructs(aiocbs, buffer, fd, blocksize);
    }

    if ((fd = TEMP_FAILURE_RETRY(close(fd)) == -1))
        error("Cannot close file");
    exit(EXIT_SUCCESS);
}