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

// void sigint_handler() {
// 	if (mq_unlink("/mq_cook"))
// 		ERR("qm_cookies unlink");
// }
int main(int argc, char **argv)
{
  // struct sigaction act;
	// memset(&act, 0, sizeof(struct sigaction));
	// act.sa_sigaction = sigint_handler;
  // int sigNO = SIGINT;
  // if (-1 == sigaction(sigNo, &act, NULL))
	// 	ERR("sigaction");


	mqd_t mq_cookies; //mq_request;
  int rand_c1, rand_c2;
	struct mq_attr attr;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 3;

  char cookiesBuff[3] = "";
  srand(getpid());
	rand_c1 = rand() % 10;
  rand_c2 = rand() % 10;

  cookiesBuff[0] = rand_c1 + '0';
  cookiesBuff[1] = rand_c2 + '0';
  cookiesBuff[2] = '\0';


	if ((mq_cookies = TEMP_FAILURE_RETRY(mq_open("/mq_cook", O_WRONLY | O_NONBLOCK | O_CREAT, 0600, &attr))) == (mqd_t)-1)
		ERR("mq_cookies open");
    
  if (TEMP_FAILURE_RETRY(mq_send(mq_cookies, cookiesBuff, 3, 1)))
				ERR("mq_cookies send");
  sleep(10);
	mq_close(mq_cookies);

	if (mq_unlink("/mq_cook"))
		ERR("qm_cookies unlink");
	return EXIT_SUCCESS;
}