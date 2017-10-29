#ifndef APP_LINK
#define APP_LINK

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <sys/stat.h> //biblioteca que permite obter o mode de um file
#include <sys/types.h>
#include <errno.h>
#include "dataLink.h"

#define DATA_CTRL_PACKET 1
#define START_CTRL_PACKET 2
#define END_CTRL_PACKET 3
#define DATA_SIZE 100
#define MAX_SIZE 255

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

typedef struct {
  unsigned int size;
  char filename[MAX_SIZE];
} FileInfo;

off_t getFileSize(char* trama, int lenghtTrama);

char* getFileName(char* trama, int lenghtTrama);

char* getFileName(char* trama, int lenghtTrama);

int connection(char* terminal, int whoCalls);

int receiveData();

int sendControlPackage(int state, FileInfo file, unsigned char *controlPacket);

int processingDataPacket(unsigned char *packet, int length, FileInfo *file, int fp);

int sendDataPackage(unsigned char *dataPacket, FILE *fp, int sequenceNumber, int *length);

int sendData();

int getFile(char *filepath);

int fileSize(FILE *fd);

#endif
