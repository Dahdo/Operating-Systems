#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t prev_signal;

void set_handler(void (*f)(int), int sigN)
{
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = f;
    if(-1 == sigaction(sigN, &action, NULL))
        ERR("sigaction");
}

void sighandler(int sig)
{
    prev_signal = sig;
}
void create_children(int n)
{
    while(n-- > 0){
        pid_t pid = fork();
        int tm = rand() % 101 + 100;
        srand(time(NULL) * pid);
        if(pid == 0){
            set_handler(SIG_DFL, SIGUSR2);
            struct timespec timesp = {0, tm * 1000000};
            for(int i = 0; i < 30; i++){
                nanosleep(&timesp, NULL);
                if(kill(getppid(), SIGUSR1))
                    ERR("kill");
                else
                    printf("%s", " *");
            }
            printf("\n");
            waitpid(0,NULL,0);
            exit(EXIT_SUCCESS);
        }

    }
}

void parent_work(sigset_t oldmask)
{
    int counter = 0;
    while(1){
        prev_signal = 0;
        while(prev_signal != SIGUSR1)
            sigsuspend(&oldmask);
        counter++;
        printf("[%d] counter: %d\n",getppid(), counter);
        if(counter >= 100){
            kill(0, SIGUSR2);
            kill(0, SIGCHLD);
            while(waitpid(0, NULL, 0) > 0);
            return;
        }

    }
}

void sigchild_handler(int sig)
{
    pid_t pid;
    while(1){
        pid = waitpid(0, NULL, WNOHANG);
        if(pid == 0)
            return;
        if(pid <= 0){
            if(errno == ECHILD)
                return;
            ERR("waitpid");
        }
    }
}

int main(int argc, char ** argv)
{
    if(argc < 2)
    exit(EXIT_FAILURE);
    int n = atoi(argv[1]);

    set_handler(sigchild_handler, SIGCHLD);
    set_handler(sighandler, SIGUSR1);
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    create_children(n);
    parent_work(oldmask);
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    return EXIT_SUCCESS;

}
