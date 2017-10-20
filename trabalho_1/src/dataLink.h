#ifndef DATA_LINK
#define DATA_LINK

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>

#define BAUDRATE B38400

#define SENDER 0
#define RECEIVER 1

#define FLAG 0x7E
#define ESC 0x7D
#define A_SENDER 0x03
#define A_RECEIVER 0x01
#define C_SET 0x03
#define C_DISC 0x0B
#define C_UA 0x07
#define C_RR 0x05
#define C_REJ 0x01


int open_serial_port(char* port, int whoCalls);

int open_receiver(char* port);

int open_sender(char* port);

int sendImportantFrame(int fd, char* frame, int length );

void insertValueAt(int index, int value, char* frame, int lenght);

void stuffing(char* frame, int *length);

void createControlFrame(char *frame, char controlByte, int whoCalls);

int llopen(char* port,int whoCalls);

int llwrite(int fd, char* buffer, int length);

int llclose(int fd);

int llread(int fd, char* buffer);

char readingArrayStatus(int fd);

#endif
