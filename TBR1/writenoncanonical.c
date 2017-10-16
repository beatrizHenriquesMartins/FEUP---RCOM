/*Non-Canonical Input Processing*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

typedef enum { START, READ, STOP } state;

int llopen_em(int fd) {
  char set[5] = {0x7E, 0x03, 0x03, 0x00, 0x7E};
  char ua[5] = {0x7E, 0x03, 0x07, 0x04, 0x7E};

  write(fd, set, strlen(set) + 1);

  int i = 0;
  while (i < 3) {
    char *buf;
    readCycle(buf);
    if (buf[3] == buf[1] ^ buf[2] && buf[2] == 0x07) {
      return fd;
    }
  }
  return -1;
}

void readCycle(int fd, char *buf) {
  int state = START;
  int res, i = 0;
  switch (state) {
  case START:
    while (res = read(fd, buf + i, 1) == 0 && buf[0] != 0x7E) {
    };
    state = READ;
    i++;
    break;
  case READ:
    while (state == READ) {
      if (res = read(fd, buf + i, i) != 0)
        if (buf[i] != 0x7E)
          i++;
        else
          state = STOP;
    }
    break;
  case STOP:
    break;
  }
}

int llwrite(int fd, char *buffer, int length){};

int llclose(int fd){};

int main(int argc, char **argv) {
  int fd, c, res;
  struct termios oldtio, newtio;
  char buf[255];
  int i, sum = 0, speed = 0;

  if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
                     (strcmp("/dev/ttyS1", argv[1]) != 0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

  fd = open(argv[1], O_RDWR | O_NOCTTY);
  if (fd < 0) {
    perror(argv[1]);
    exit(-1);
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

  newtio.c_cc[VTIME] = 0; /* inter-character timer unused */
  newtio.c_cc[VMIN] = 5;  /* blocking read until 5 chars received */

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

  llopen_em(fd);

  if (tcsetattr(fd, TCSANOW, &oldtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  close(fd);
  return 0;
}
