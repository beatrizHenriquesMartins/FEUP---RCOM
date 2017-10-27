#ifndef APP_LINK
#define APP_LINK

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include "dataLink.h"

#define FILE_SIZE_BYTE 0
#define FILE_NAME_BYTE 1
#define FILE_PERMISSIONS_BYTE 2

#define PACKET_SIZE 256
#define PACKET_HEADER_SIZE 4
#define PACKET_DATA_SIZE PACKET_SIZE - PACKET_HEADER_SIZE
#define FILE_SIZE 10968

typedef struct{
  int fileDescriptor; /*Descritor correspondente à porta série*/
  int status; /*TRANSMITTER | RECEIVER*/
}app;

app application;

int getFileSize(char *trama, int lenghtTrama);

char *getFileName(char *packet, int packet_len);

int connection(char*terminal,int whoCalls);

int receiveData();

/*
send data
app layer
llopen
send control package
processing data packet
senda data package
*/

#endif
