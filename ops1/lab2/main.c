#include <stdio.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define ERR(source)                                                                                                    \
	(fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t prev_signal;

void set_handler(void (*f)(int), int sigN)
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = f;
    if(-1 == sigaction(sigN, &act, NULL))
        ERR("sigaction");
}

void sighandler(int sig)
{
    prev_signal = sig;
}

void create_children(int n){

    while(n-- > 0){

        pid_t pid = fork();
        int k = rand() % 11;
        srand(time(NULL) * pid);
    

        if(pid == 0){
            set_handler(sighandler, SIGUSR1);
            int i;
            for(int s = 0; s < k; s++){
                prev_signal = 0;
                sleep(1);
                kill(0, SIGUSR1);
                if(prev_signal == SIGUSR1)
                    i++;
            }         
            prev_signal = 0;

            printf("[%d], k: %d, i: %d\n", getpid(), k, i);

        //     struct info{
        //     int i;
        //     int k;
        // };
            // struct info proc_inf;
            // proc_inf.i = i;
            // proc_inf.k = k;
            exit(EXIT_SUCCESS);
        }
    }
}

 
int countDigit(int n)
{
    if (n == 0)
        return 1;
    int count = 0;
    while (n != 0) {
        n = n / 10;
        ++count;
    }
    return count;
}

int main(int argc, char ** argv)
{
    if(argc < 2)
        exit(EXIT_FAILURE);
    int n = atoi(argv[1]);

    create_children(n);
    int status;
    int  out = open("out.txt", O_RDWR | O_APPEND);
    while(waitpid(0, &status, WNOHANG) > 0){
        int digNum = countDigit(status);
        char * buf = malloc(sizeof(digNum));

        for(int i = digNum - 1; i >= 0; i--)
            *(buf + i) = status % 10;

       // printf("%d \n", status);
        write(out, buf, digNum);
       
    }
     close(out);
    exit(EXIT_SUCCESS);
}