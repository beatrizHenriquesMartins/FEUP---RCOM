/*Non-Canonical Input Processing*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>

// dew
#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define TRANSMITTER 0x03
#define RECEIVER 0x07
#define FLAGSET 0x7E

volatile int STOP = FALSE;

int llopen(int port, char flag) {
  int fd = open(port, O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(port);
    return (-1);
  }

<<<<<<< HEAD
  if (tcgetattr(fd, &oldtio) == -1) { /* save current port settings */
    perror("tcgetattr");
    return (-1);
  }

  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;
=======

  /*
    int fd = open(port, O_RDWR | O_NOCTTY );
    if (fd <0) {perror(port); return(-1); }

    if ( tcgetattr(fd,&oldtio) == -1) { // save current port settings
      perror("tcgetattr");
      return(-1);
    }
>>>>>>> origin/master

  /* set input mode (non-canonical, no echo,...) */
  newtio.c_lflag = 0;
  if (flag == TRANSMITTER) {
  } else if (flag == RECEIVER) {
  }

<<<<<<< HEAD
  newtio.c_cc[VTIME] = 1;
  newtio.c_cc[VMIN] = 0;

  return fd;
}

int llread(int fd, char *buffer) {
  int i = 0;
  while (STOP == FALSE) {          /* loop for input */
    res = read(fd, buffer + i, 1); /* returns after 1 chars have been input */
    if (res > 0) {                 /* so we can printf... */
      if (buffer[i] == '\0')
        STOP = TRUE;
      i++;
=======
    //set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    if(flag == TRANSMITTER) {
    } else if (flag == RECEIVER) {
    }

    newtio.c_cc[VTIME]    = 1;   // inter-character timer unused
    newtio.c_cc[VMIN]     = 0;   // blocking read until 5 chars received /
*/

int llopen(int port){
  //mudar port para fd
    char set[5] = {0x7E, 0x03,0x03,0x00,0x7E};
    char ua[5] = {0x7E, 0x03,0x07,0x04,0x7E};

    int i = 0;
    int res;
    char* buffer;

    write(port, set);
    while (STOP == FALSE) {       /* loop for input */
      res = read(port,buffer+i,1);   /* returns after 5 chars have been input */
      if(res > 0) {		/* so we can printf... */
           if (buffer[i] == 0x7E){

           }
           i++;
       }
    }

    res = write(fd,buffer,strlen(buffer)+1);
    printf("%d bytes written\n", res);

    printf(":%s:%d\n", buffer, strlen(buf));

    return res;
    return fd;
}

int llread(int fd, char *buffer){
    int i = 0;
    while (STOP==FALSE) {       /* loop for input */
      res = read(fd,buffer+i,1);   /* returns after 1 chars have been input */
      if(res > 0) {		/* so we can printf... */
           if (buffer[i]=='\0') STOP=TRUE;
	   i++;
	}
>>>>>>> origin/master
    }
  }
  printf(":%s:%d\n", buffer, strlen(buffer));

<<<<<<< HEAD
  res = write(fd, buf, strlen(buffer) + 1);
  printf("%d bytes written\n", res);
  return res;
=======
    res = write(fd,buf,strlen(buffer)+1);
    printf("%d bytes written\n", res);
    return res;
>>>>>>> origin/master
}

int llclose(int fd) {}

<<<<<<< HEAD
int main(int argc, char **argv) {
  int c, res;
  struct termios oldtio, newtio;
  char buf[255];
=======
    if ( (argc < 2) ||
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) &&
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }
>>>>>>> origin/master

  if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
                     (strcmp("/dev/ttyS1", argv[1]) != 0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
<<<<<<< HEAD
  if (fd = llopen(argv[1], RECEIVER) == -1)
    exit(-1);
=======
   if(fd = llopen(argv[1],RECEIVER)== -1)
      exit(-1);

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */
>>>>>>> origin/master

  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  llread(fd, buf);

  /*
    O ciclo WHILE deve ser alterado de modo a respeitar o indicado no gui�o
  */

  sleep(1);
  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  return 0;
}
