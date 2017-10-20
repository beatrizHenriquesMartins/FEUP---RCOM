#include "dataLink.h"


struct termios oldtio,newtio;

int open_serial_port(char* port, int whoCalls){
  int fd;
  char serialPort[10] = "/dev/ttyS";
  strcat(serialPort,port);

  if ((strcmp("/dev/ttyS0", serialPort)!=0) && (strcmp("/dev/ttyS1", serialPort)!=0)) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
  }

  fd = open(serialPort, O_RDWR | O_NOCTTY);

  if (fd < 0) {
    perror(serialPort); exit(-1);
  }

  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;

  if (whoCalls == SENDER){
    newtio.c_cc[VTIME]    = 1;
    newtio.c_cc[VMIN]     = 0;
  }else if (whoCalls == RECEIVER){
    newtio.c_cc[VTIME]    = 0;
    newtio.c_cc[VMIN]     = 1;
  }

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  return fd;
}

int open_receiver(char* port){
  int fd;
  fd = open_serial_port(port,RECEIVER);
  char controlByte = readingArrayStatus(fd);

  return fd;
}

int open_sender(char* port){
  char * set_frame = NULL;

  int fd = open_serial_port(port,SENDER);

  createControlFrame(set_frame,C_SET,SENDER);

  sendImportantFrame(fd,set_frame,5);

  return fd;
}

int sendImportantFrame(int fd, char* frame, int length ){
  stuffing(frame,&length);
  int res;
  int tries = 0;
  do{
    res = write(fd,frame,length);
    sleep(1);
    tries++;
  }while (res < 0 && tries < 3);
  if (tries == 3){
    perror("Can not write to serial port");
    return -1;
  }
  return 0;
}

void insertValueAt(int index, int value, char* frame, int lenght){
  int i;
  for (i = lenght -1; i >= index; i--){
    frame[i+1] = frame[i];
  }
  frame[i] = value;
}

void stuffing(char* frame, int *length){
  int i;
  int allocated_space = *length;
  for ( i = 0; i < *length; i++){
    if (frame[i] == ESC || frame[i] == FLAG){
      if ((*length) >= allocated_space){
        allocated_space = 2 * (*length);
        frame = (char *) realloc(frame, allocated_space);
      }
      frame[i] = frame[i] ^ 0x20;
      insertValueAt(i,ESC,frame,*length);
      (*length)++;
      i++;
    }
  }
}

void createControlFrame(char *frame, char controlByte, int whoCalls){
  int isAnswer = 0;
  frame = malloc(5 * sizeof(char));
  frame[0] = FLAG;

  //verificação resposta ou cmd
  if(controlByte == C_UA || controlByte == C_RR || controlByte == C_REJ ){
    isAnswer=1;
  }

  if (whoCalls == SENDER){
    (isAnswer) ? (frame[1] = A_RECEIVER) : (frame[1] = A_SENDER);
  }else{
    (isAnswer) ? (frame[1] = A_SENDER) : (frame[1] = A_RECEIVER);
  }
  
  frame[2] = controlByte;
  frame[3] = frame[1] ^ frame[2];
  frame[4] = FLAG;
}

char readingArrayStatus(int fd){
  int state = 0;
  char frame_receive[5];
  char var;

  while(state != 5){
    int res = read(fd,&var,1);
    frame_receive[state] = var;
    if(res > 0){
      switch (state) {
        case 0:{
          if(var == FLAG){
            state = 1;
          }
          break;
        }
        case 1:{
          if(var != FLAG){
            state = 2;
          }else{
            state =  1;
          }
          break;
        }
        case 2:{
          if(var != FLAG){
            state = 3;
          }else{
            state =  1;
          }
          break;
        }
        case 3:{
          if(var == (frame_receive[2]^frame_receive[1])){
            state = 4;
          }else{
            perror("Damage package");
            return -1;
          }
          break;
        }
        case 4:{
          if(var != FLAG){
            state = 0;
          }else{
            state =  5;
          }
          break;
        }
      }
    }
    return frame_receive[2];
  }
  perror("Damage package");
  return -1;
}

int llopen(char* port, int whoCalls){
  if (whoCalls == RECEIVER){
    open_receiver(port);
  }else if (whoCalls == SENDER){
    open_sender(port);
  }else{
    return -1;
  }
  return 0;
}
