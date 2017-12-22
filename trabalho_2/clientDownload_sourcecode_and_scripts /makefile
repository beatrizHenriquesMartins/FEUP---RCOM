CC=gcc
CFLAGS=-Wall -I.

download: clientTCP.c
	$(CC) clientTCP.c tcp.c parser.c -o download $(CFLAGS)

clean:
	rm -f download *.o
