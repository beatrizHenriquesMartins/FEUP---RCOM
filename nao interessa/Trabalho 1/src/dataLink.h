#pragma once

#include <termios.h>

#define FLAG 0x7E
#define A_SENDER      0x03
#define A_RECEIVER    0x01
#define C_SET         0x03
#define C_DISC        0x0B
#define C_UA          0x07
#define C_RR_0        0x05
#define C_RR_1        0x85
#define C_REJ_0       0x01
#define C_REJ_1       0x81
#define ESCAPE        0x7D
#define STUFF_BYTE    0x20

#define FALSE 0
#define TRUE 1

#define BAUDRATE      B9600
#define MAX_SIZE      256
#define N_TRIES       5
#define TIMEOUT       3
#define PACKET_MAX_SIZE MAX_SIZE - 6
#define DATA_MAX_SIZE PACKET_MAX_SIZE - 4

typedef enum {
  START, FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP
} state;

typedef enum {
  SET, UA, RR, REJ
} cmd;

typedef struct  {
  int baudRate;/*Velocidade de transmissão*/
  unsigned int sequenceNumber;   /*Número de sequência da trama: 0, 1*/
  unsigned int timeout;/*Valor do temporizador: 1 s*/
  unsigned int numTransmissions; /*Número de tentativas em caso defalha*/
  char *frame;/*Trama*/
} linkLayer;

extern LinkLayer* ll;
