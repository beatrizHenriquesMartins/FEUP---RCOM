/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

/* TP1 */
// VTIME e VMIN alterados
#define VTIME 1
#define VMIN 0

/* TP2 */
#define FLAG_SET 0x7E
#define TRANSMITTER 0x03
#define RECEIVER 0x07

volatile int STOP = FALSE;

int llopen(int port, int flag){
  int fd = open(argv[1], O_RDWR | O_NOCTTY );
  if (fd <0) {perror(argv[1]); exit(-1); }

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

  newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
  newtio.c_cc[VMIN]     = 5;   /* blocking read until 5 chars received */

  return fd;
}

int llread(int fd, char *buffer){
  int i = 0;
  int res;

  while (STOP == FALSE) {       /* loop for input */
    res = read(fd,buffer+i,1);   /* returns after 5 chars have been input */
    if(res > 0) {		/* so we can printf... */
         if (buffer[i]=='\0'){
           STOP = TRUE;
         }
         i++;
     }
  }

  res = write(fd,buffer,strlen(buffer)+1);
  printf("%d bytes written\n", res);

  printf(":%s:%d\n", buffer, strlen(buf));

  return res;
}

int main(int argc, char** argv){
    int fd;
    int c;
    int res;
    struct termios oldtio, newtio;
    char buf[255];
    int i;
    int sum = 0;
    int speed = 0;

    if ( (argc < 2) || ((strcmp("/dev/ttyS0", argv[1])!= 0) && (strcmp("/dev/ttyS1", argv[1])!= 0))) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }


  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    /*
    fd = open(argv[1], O_RDWR | O_NOCTTY );
    if (fd <0) {
      perror(argv[1]);
      exit(-1);
    }

    if ( tcgetattr(fd,&oldtio) == -1) { // save current port settings
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   // inter-character timer unused
    newtio.c_cc[VMIN]     = 5;   // blocking read until 5 chars received
    */

    fd = llopen(argv[1],RECEIVER);
    if(fd == -1){
      exit(-1);
    }


  /*
    VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a
    leitura do(s) pr�ximo(s) caracter(es)
  */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

    printf("New termios structure set\n");

    llread(fd, buf);

/* TP1 */
    //Escrever para o buffer
   /* for (i = 0; i < 255; i++) {
      buf[i] = 'a';
    }*/

    /*
    gets(buf);

    res = write(fd,buf,strlen(buf)+1);
    printf("%d bytes written\n", res);

    i = 0;
    while (STOP==FALSE) {       /* loop for input */
      res = read(fd,buf+i,1);   /* returns after 5 chars have been input */
      if(res > 0) {		/* so we can printf... */
           if (buf[i]=='\0'){
             STOP=TRUE;
           }
	         i++;
	     }
    }
    printf(":%s:%d\n", buf, strlen(buf));
    */

  /*
    O ciclo FOR e as instru��es seguintes devem ser alterados de modo a respeitar
    o indicado no gui�o
  */

    if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }

/* TP2 */

    close(fd);
    return 0;
}
