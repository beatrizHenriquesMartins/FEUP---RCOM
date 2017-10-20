#include "dataLink.h"


struct termios oldtio,newtio;

int open_serial_port(char* port, int whoCalls){

  int fd;
  char* serialPort = "/dev/ttyS";
  strcat(serialPort,port);
  if ((strcmp("/dev/ttyS0", serialPort)!=0) &&
  	      (strcmp("/dev/ttyS1", serialPort)!=0)) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
  }
  fd = open(serialPort, O_RDWR | O_NOCTTY );
  if (fd <0) {
    perror(serialPort); exit(-1);
  }
  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
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

  if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }
  return fd;
}

int open_receiver(char* port){
  int fd;
  fd = open_serial_port(port,RECEIVER);
  return fd;
}

int open_sender(char* port){
  int fd = open_serial_port(port,SENDER);
  return fd;
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
