#define _GNU_SOURCE
#include <errno.h>
#include <mqueue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>


#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

int digit_count(int n)
{
    if (n / 10 == 0)
        return 1;
    return 1 + digit_count(n / 10);
}

int main(int argc, char **argv)
{
	mqd_t mq_client;
	struct mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 1;
    
    pid_t pid = getpid();
    int dig_count = digit_count(pid);

    char * buffer = malloc(dig_count + 2); // 2 for / and null-term char
        if(NULL == buffer)
            ERR("can't allocatte buffer");

    *buffer = '/';
    sprintf(buffer + 1, "%d", pid);

	if ((mq_client = TEMP_FAILURE_RETRY(mq_open(buffer, O_RDWR | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_client open");
    printf("%s\n", buffer);
    system("pidof server");
    sleep(1);

	mq_close(mq_client);

	if (mq_unlink(buffer))
		ERR("mq_client unlink");
    
	return EXIT_SUCCESS;
}