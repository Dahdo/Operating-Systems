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

#define BACKLOG 3

volatile sig_atomic_t work = 1;
void siginthandler(int sig)
{
    work = 0;
}

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0x00, sizeof(struct sigaction));
    act.sa_handler = f;

    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

ssize_t bulk_read(int fd, char *buf, size_t count)
{
    int c;
    size_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(read(fd, buf, count));
        if (c < 0)
            return c;
        if (c == 0)
            return len;
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}

ssize_t bulk_write(int fd, char *buf, size_t count)
{
    int c;
    size_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(write(fd, buf, count));
        if (c < 0)
            return c;
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}

int make_socket(int domain, int type)
{
    int sock;
    sock = socket(domain, type, 0);
    if (sock < 0)
        ERR("socket");
    return sock;
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

void usage(char *name)
{
    fprintf(stderr, "USAGE: %s port\n", name);
    exit(EXIT_FAILURE);
}

void dowork(int socket)
{
    int clientfd;
    fd_set base_rfds, rfds;
    FD_ZERO(&base_rfds);
    FD_SET(socket, &base_rfds);
    sigset_t emptyset;
    sigemptyset(&emptyset);
    size_t size;
    char buffer[8 * sizeof(double)];
    if (NULL == buffer)
        ERR("malloc buffer");

    while (work)
    {
        rfds = base_rfds;
        if (pselect(socket + 1, &rfds, NULL, NULL, NULL, &emptyset) > 0)
        {
            if ((clientfd = add_new_client(socket)) == -1)
                continue;
            if ((size = TEMP_FAILURE_RETRY(recv(clientfd, buffer, 8 * sizeof(double), MSG_WAITALL))) == -1)
                ERR("read");
            // char* ptr;
            // for(int i = 0; i < 8; i++) {
            //     sprintf("%d", strtod(buffer, &ptr));
            // }
            ERR("read");
        }
        else
        {
            if (EINTR == errno)
                continue;
            ERR("pselect");
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    sigset_t mask, oldmask;
    if (argc != 2)
        usage(argv[0]);

    sethandler(SIG_IGN, SIGPIPE);
    sethandler(siginthandler, SIGINT);
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    int socket = bind_tcp_socket(atoi(argv[1]));
    int new_flags = fcntl(socket, F_GETFL) | O_NONBLOCK;
    if (fcntl(socket, F_SETFL, new_flags) == -1)
        ERR("fcntl");
    if (TEMP_FAILURE_RETRY(close(socket)) < 0)
        ERR("close");
    return EXIT_SUCCESS;
}
