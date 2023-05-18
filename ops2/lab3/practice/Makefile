CC=gcc
CFLAGS= -std=gnu99 -Wall

compile: localClient.c localServer.c tcpClient.c tcpServer.c udpClient.c udpServer.c
	$(CC) $(CFLAGS) -o localClient localClient.c
	$(CC) $(CFLAGS) -o localServer localServer.c
	$(CC) $(CFLAGS) -o tcpServer tcpServer.c
	$(CC) $(CFLAGS) -o tcpClient tcpClient.c
	$(CC) $(CFLAGS) -o udpServer udpServer.c
	$(CC) $(CFLAGS) -o udpClient udpClient.c

clear:
	-rm -rf localClient localServer tcpClient tcpServer udpClient udpServer