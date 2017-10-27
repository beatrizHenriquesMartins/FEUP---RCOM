#ifndef DATA_LINK
#define DATA_LINK

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BAUDRATE B38400

#define SENDER 0
#define RECEIVER 1

#define A_SENDER 0x03
#define A_RECEIVER 0x01

#define C_SET 0x03
#define C_DISC 0x0B
#define C_UA 0x07
#define C_RR 0x05
#define C_REJ 0x01
#define ESC 0x7D
#define ESC_HIDE_BYTE 0x5D
#define FLAG 0x7E
#define FLAG_HIDE_BYTE 0x5E
#define N_OF_SEQ_0 0x00
#define N_OF_SEQ_1 0x40

#define DATA_BYTE 1
#define START_BYTE 2
#define END_BYTE 3

/* complete */
void retry();

/* complete */
void atende();

/* complete */
int open_serial_port(char* port, int whoCalls);

/* complete */
int open_receiver(char* port);

/* complete */
int open_sender(char* port);

/* complete */
void createControlFrame(char *frame, char controlByte, int whoCalls);

/* complete
   maquina de estados */
char readingArrayStatus(int fd);

/* complete */
void insertValueAt(int index, int value, char* frame, int lenght);

/* complete */
void shiftBack(int index, char *frame, int frameSize);

/* complete */
char getBCC2(unsigned char *frame, unsigned int length);

/* complete */
int stuffing(unsigned char *frame, int length);

/* complete */
int destuffing(char *frame);

/* complete */
int processingDataFrame(char *frame);

/* complete */
int readingFrame(int fd, char *frame);

/**/
int resetSettings(int fd);

/* complete */
int llopen(char* port,int whoCalls);

/**/
int llread(int fd, char* buffer);

/**/
int llwrite(int fd, char* buffer, int length);

/**/
//int llclose(int fd, int whoCalls);



#endif
