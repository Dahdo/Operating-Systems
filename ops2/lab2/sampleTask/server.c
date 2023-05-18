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
	mqd_t mq_s, mq_d, mq_m, mq_server;
	struct mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = sizeof(pid_t) + 2 * sizeof(int);
    int * readBuffer = malloc(sizeof(pid_t) + 2 * sizeof(int));
    if(NULL == readBuffer)
        ERR("can't allocate readBuff");
    
    pid_t pid = getpid();
    int dig_count = digit_count(pid);
    char * buffer = malloc(dig_count + 4); // for /_s and null term char
        if(NULL == buffer)
            ERR("can't allocatte buffer");
    *buffer = '/';
    sprintf(buffer + 1, "%d", pid);
    strcat(buffer, "_s");

	if ((mq_s = TEMP_FAILURE_RETRY(mq_open(buffer, O_RDONLY | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_s open");
    printf("%s\n", buffer);

    buffer[strlen(buffer) - 1] = 'd';
	if ((mq_d = TEMP_FAILURE_RETRY(mq_open(buffer, O_RDONLY | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_d open");
    printf("%s\n", buffer);

    buffer[strlen(buffer) - 1] = 'm';
    if ((mq_m = TEMP_FAILURE_RETRY(mq_open(buffer, O_RDONLY | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_m open");
    printf("%s\n", buffer);

    if ((mq_server = TEMP_FAILURE_RETRY(mq_open("mq_server", O_WRONLY | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_server open");

    if (mq_receive(mq_s, (char *)readBuffer, sizeof(sizeof(pid_t) + 2 * sizeof(int)), NULL) < 1)
			ERR("mq_receive");
    
    sleep(1);

	mq_close(mq_s);
	mq_close(mq_d);
    mq_close(mq_m);

	if (mq_unlink(buffer))
		ERR("qm unlink");
    
    buffer[strlen(buffer) - 1] = 'd';
	if (mq_unlink(buffer))
		ERR("qd unlink");
    
    buffer[strlen(buffer) - 1] = 's';
    if (mq_unlink(buffer))
		ERR("qs unlink");
	return EXIT_SUCCESS;
}