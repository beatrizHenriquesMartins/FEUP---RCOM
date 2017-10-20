#ifndef DATA_LINK
#define DATA_LINK

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define BAUDRATE B38400

#define SENDER 0
#define RECEIVER 1

int open_serial_port(char* port, int whoCalls);

int open_receiver(char* port);

int open_sender(char* port);

int llopen(char* port,int whoCalls);

int llwrite(int fd, char* buffer, int length);

int llclose(int fd);

int llread(int fd, char* buffer);

#endif
