#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define MSG_SIZE (PIPE_BUF - sizeof(pid_t))
#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t last_signal = 0;

void sig_handler(int sig)
{
	last_signal = sig;
}

void sig_killme(int sig)
{
	exit(EXIT_SUCCESS);
}

int sethandler(void (*f)(int), int sigNo)
{
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (-1 == sigaction(sigNo, &act, NULL))
		return -1;
	return 0;
}

void sigchld_handler(int sig)
{
	pid_t pid;

	for (;;) {
		pid = waitpid(0, NULL, WNOHANG);
		if (0 == pid)
			return;
		if (0 >= pid) {
			if (ECHILD == errno)
				return;
			ERR("waitpid:");
		}
	}
}

void child_work(int childP0, int parentP1, char * str_arg, int str_size)
{
    if (sethandler(sig_killme, SIGINT))
		ERR("Setting SIGINT handler in child");

    for(;;) {
        //writing to parent pipe
        unsigned char c;
        char buffer[PIPE_BUF];
        char * buf;

        *((pid_t *)buffer) = getpid();
        buf = buffer + sizeof(pid_t);

        c = (unsigned char)childP0;
        buffer[sizeof(pid_t) + 1] = c;
        strcat(buffer, "child_work");

        srand(getpid() + rand());
        int offset = rand() % str_size;
        char substr [str_size - offset];
        substr[0] = '\0';
        strcat(substr, str_arg + offset);

        strcat(buffer, substr);
        memset(buf + sizeof(pid_t) + 11 + str_size - offset, 0, MSG_SIZE - 11 + str_size - offset);

        if (write(parentP1, &buffer, PIPE_BUF) < 0)
            ERR("write to parentP1");


        //reading from parent pipe    
        char readbuff[PIPE_BUF];
        if(read(childP0, readbuff, PIPE_BUF) <= 0)
                ERR("read child");
        printf("[Child] %d:", *((pid_t*)readbuff));
        printf("%d:", readbuff[sizeof(pid_t)]);
        printf("%s\n", readbuff + sizeof(pid_t) - 1); 
    }  

}

void parent_work(int childP1, int parentP0, char * str_arg, int str_size)
{
    for(;;) {
        unsigned char c;
        char buffer[PIPE_BUF];
        char * buf;
        
        //reading from child pipe
        char readbuff[PIPE_BUF];
        if(read(parentP0, readbuff, PIPE_BUF) < 0)
                ERR("read parent");
        printf("[Parent] %d:", *((pid_t*)readbuff));
        printf("%d:", readbuff[sizeof(pid_t)]);
        printf("%s\n", readbuff + sizeof(pid_t) - 1);

        //write to child pipe

        *((pid_t *)buffer) = getpid();
        buf = buffer + sizeof(pid_t);

        c = (unsigned char)parentP0;
        buffer[sizeof(pid_t) + 1] = c;
        strcat(buffer, "parent_work");
        
        srand(getpid() + rand());
        int offset = rand() % str_size;
        char substr [str_size - offset];
        substr[0] = '\0';
        strcat(substr, str_arg + offset);

        strcat(buffer, substr);
        memset(buf + sizeof(pid_t) + 12 + str_size - offset, 0, MSG_SIZE - 12 + str_size - offset);

        if (write(childP1, &buffer, PIPE_BUF) < 0)
        ERR("write to parentP1");
    }
}

void create_children_and_pipes(int *childP1, int parentP1, char *str_arg, int str_size)
{
	int tmpfd[2];
		if (pipe(tmpfd))
			ERR("pipe");

		switch (fork()) {
		case 0:

			if (close(tmpfd[1]))
				ERR("close");

			child_work(tmpfd[0], parentP1, str_arg, str_size);
			if (close(tmpfd[0]))
				ERR("close");

			if (close(parentP1))
				ERR("close");
			exit(EXIT_SUCCESS);

		case -1:
			ERR("Fork:");
		}
        *childP1 = tmpfd[1];
		if (close(tmpfd[0]))
			ERR("close");
}

void usage(char *name)
{
	fprintf(stderr, "USAGE: %s n\n", name);
	fprintf(stderr, "0<n<=10 - number of children\n");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	int* childP1, parentPs[2];
    childP1 = malloc(sizeof(int));
	 if (2 != argc)
		usage(argv[0]);

    if (sethandler(sigchld_handler, SIGCHLD))
		ERR("Setting parent SIGCHLD:");

    if(sethandler(SIG_IGN, SIGINT))
    ERR("ignoring sigint");

	char str_arg[strlen(argv[1] + 1)];
    str_arg[0] = '\0';
    strcat(str_arg, argv[1]);

	if (pipe(parentPs))
		ERR("pipe");
	create_children_and_pipes(childP1, parentPs[1], str_arg, strlen(argv[1] + 1));
	if (close(parentPs[1]))
		ERR("close");
	parent_work(*childP1, parentPs[0], str_arg, strlen(argv[1] + 1));
	if (childP1 && close(*childP1))
			ERR("close");
	if (close(parentPs[0]))
		ERR("close");
	return EXIT_SUCCESS;
}