#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define BACKLOG 25
#define THREAD_NUM 6
volatile sig_atomic_t work = 1;

typedef struct
{
    uint8_t floor_elev1;
    uint8_t floor_elev2;
    uint8_t num_people_elev1;
    uint8_t num_people_elev2;
} thread_arg;

typedef struct
{
    thread_arg* elevators;
    int clientfd;
} connection_arg;

void siginthandler(int sig)
{
    work = 0;
}

void usage(char *name)
{
    fprintf(stderr, "USAGE: %s port", name);
    exit(EXIT_FAILURE);
}

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0x00, sizeof(struct sigaction));
    act.sa_handler = f;

    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

int make_socket(int domain, int type)
{
    int sock;
    sock = socket(domain, type, 0);
    if (sock < 0)
        ERR("socket");
    return sock;
}

void cleanup(void *arg)
{
    pthread_mutex_unlock((pthread_mutex_t *)arg);
}

void *elev_function(void *arg)
{
    // int clientfd;
    // thread_arg targ;
    // memcpy(&targ, arg, sizeof(targ));
    // while (1)
    // {
     printf("elevator thread created!\n");
    //}
    return NULL;
}

void *conn_function(void *arg)
{
    connection_arg* con_arg;
    memcpy(&con_arg, arg, sizeof(arg));
    int* clientfd = con_arg->clientfd;

    printf("connection thread created!\n");
    uint8_t buffer[3];
    uint8_t lev1 = con_arg->elevators->floor_elev1;
    uint8_t lev2 = con_arg->elevators->floor_elev2;
    buffer[0] = lev1;
    buffer[1] = lev2;

    if (TEMP_FAILURE_RETRY(send(clientfd, ((char*) &buffer), 3, 0)) == -1)
            ERR("write");
    free(arg);
    return NULL;
}

int bind_tcp_socket(uint16_t port)
{
    struct sockaddr_in addr;
    int socketfd, t = 1;
    socketfd = make_socket(PF_INET, SOCK_STREAM);
    memset(&addr, 0x00, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)))
        ERR("setsockopt");
    if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        ERR("bind");
    if (listen(socketfd, BACKLOG) < 0)
        ERR("listen");
    return socketfd;
}

int add_new_client(int sfd)
{
    int nfd;
    if ((nfd = TEMP_FAILURE_RETRY(accept(sfd, NULL, NULL))) < 0)
    {
        if (EAGAIN == errno || EWOULDBLOCK == errno)
            return -1;
        ERR("accept");
    }
    return nfd;
}



void dowork(int socket)
{
    int clientfd;
    thread_arg* elev_arg = malloc(sizeof(thread_arg));
    if(NULL == elev_arg)
        ERR("malloc args");
    elev_arg->floor_elev1 = 0;
    elev_arg->floor_elev2 = 0;
    
    pthread_t elev_thread;
    if (pthread_create(&elev_thread, NULL, elev_function, (void *)elev_arg) != 0)
            ERR("pthread_create");
    int n = 10;
    while (n > 0)
    {
        if ((clientfd = add_new_client(socket)) == -1)
            ERR("add client");

            pthread_t conn_thread;
            connection_arg* conn_arg = malloc(sizeof(connection_arg));
            if(NULL == conn_arg)
                ERR("malloc args");
        if (pthread_create(&conn_thread, NULL, conn_function, (void *)conn_arg) != 0)
            ERR("pthread_create");
       n--; 
    }
    free(elev_arg);
}
int main(int argc, char **argv)
{
    if (argc != 2)
        usage(argv[0]);

    sethandler(siginthandler, SIGINT);
    int socket = bind_tcp_socket(atoi(argv[1]));
    
    dowork(socket);

    if (TEMP_FAILURE_RETRY(close(socket)) < 0)
        ERR("close");
    return EXIT_SUCCESS;
}
