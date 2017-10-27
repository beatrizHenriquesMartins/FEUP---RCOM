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

struct applicationLayer {
int fileDescriptor; /*Descritor correspondente à porta série*/
int status; /*TRANSMITTER | RECEIVER*/
}app;

app application;

struct linkLayer {
char port[20]; /*Dispositivo /dev/ttySx, x = 0, 1*/
int baudRate; /*Velocidade de transmissão*/
unsigned int sequenceNumber; /*Número de sequência da trama: 0, 1*/
unsigned int timeout; /*Valor do temporizador: 1 s*/
unsigned int numTransmissions; /*Número de tentativas em caso de
falha*/
char frame[MAX_SIZE]; /*Trama*/
}link;


int test_connection(char*terminal,int whoCalls);
/*
send data
app layer
llopen
send control package
processing data packet
senda data package
*/

#endif
