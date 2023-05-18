#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define BACKLOG 3


int sethandler(void (*f)(int), int sigNo)
{
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_handler = f;
	if (-1 == sigaction(sigNo, &act, NULL))
		return -1;
	return 0;
}

int make_socket(int domain, int type)
{
	int sock;
	sock = socket(domain, type, 0);
	if (sock < 0)
		ERR("socket");
	return sock;
}

int bind_inet_socket(uint16_t port, int type)
{
	struct sockaddr_in addr;
	int socketfd, t = 1;
	socketfd = make_socket(PF_INET, type);
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)))
		ERR("setsockopt");
	if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		ERR("bind");
	return socketfd;
}

ssize_t bulk_write(int fd, char *buf, size_t count)
{
	int c;
	size_t len = 0;
	do {
		c = TEMP_FAILURE_RETRY(write(fd, buf, count));
		if (c < 0)
			return c;
		buf += c;
		len += c;
		count -= c;
	} while (count > 0);
	return len;
}

char get_input() {
	char c;
	scanf("%c", &c);
	return c;
}


void doServer(int fd, int32_t key)
{
	struct sockaddr_in addr;
	socklen_t size = sizeof(addr);
	char buf[sizeof(int32_t)];
	int32_t readNum;

	char readChar = get_input();
	while(readChar != 'E') {
		if(readChar == 'E')
			return;
		if (TEMP_FAILURE_RETRY(recvfrom(fd, buf, sizeof(int32_t), 0, &addr, &size) < 0))
			ERR("rcvfrom:");
		readNum = ntohl(*((int32_t *)buf));
		if(readChar == 'P')
			printf("Server: %d\n", readNum);
		int32_t modifiedNum = (readNum + key) % 100000000;
		*((int32_t*)buf) = htonl(modifiedNum);
		if(readChar == 'E')
			return;
		if (TEMP_FAILURE_RETRY(sendto(fd, buf, sizeof(int32_t), 0, &addr, size)) < 0)
				ERR("sendfrom:");

		readChar = get_input();
		
	}
	
}



void usage(char *name)
{
	fprintf(stderr, "USAGE: %s port\n", name);
}

int main(int argc, char **argv)
{
	int fd;
	if (argc != 4) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	int32_t key = atoi(argv[1]);
	int32_t prob = atoi(argv[2]);

	if (sethandler(SIG_IGN, SIGPIPE))
		ERR("Seting SIGPIPE:");
	fd = bind_inet_socket(atoi(argv[3]), SOCK_DGRAM);
	doServer(fd, key);
	if (TEMP_FAILURE_RETRY(close(fd)) < 0)
		ERR("close");
	fprintf(stderr, "Server has terminated.\n");
	return EXIT_SUCCESS;
}