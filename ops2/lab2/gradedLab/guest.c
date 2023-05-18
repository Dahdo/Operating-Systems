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

void getCookies(mqd_t mq_cookies) {
    int noCook = 0;
    char cookiesBuff[3] = "";

    while(1) {
        int count = TEMP_FAILURE_RETRY(mq_receive(mq_cookies, cookiesBuff, 3, NULL));
        if (count > 0) {
            printf("cookie: %c, %c\n", cookiesBuff[0], cookiesBuff[1]);
            return;
        }
		if (count <= 0) {

			if (errno == EAGAIN)
                noCook++;
            else {
                mq_close(mq_cookies);
                if (mq_unlink("/mq_cook"))
		            ERR("qm_cookies unlink");

                ERR("get cookies");
            }
                
		}

        if(noCook == 3) {
            printf("No cookies for me");
            mq_close(mq_cookies);
                if (mq_unlink("/mq_cook"))
		            ERR("qm_cookies unlink");

            exit(EXIT_FAILURE);
        }
        

    }
}

int main(int argc, char **argv)
{
    mqd_t mq_cookies;
    struct mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 3;

    //char cookiesBuff[3] = "";

	if ((mq_cookies = TEMP_FAILURE_RETRY(mq_open("/mq_cook", O_RDONLY | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_cookies open");
				

    //printf("cookie: %c, %c\n", cookiesBuff[0], cookiesBuff[1]);
    getCookies(mq_cookies);

	mq_close(mq_cookies);

    if (mq_unlink("/mq_cook"))
		ERR("qm_cookies unlink");

	return EXIT_SUCCESS;
}