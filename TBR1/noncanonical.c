/*Non-Canonical Input Processing*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

typedef enum { START, READ, STOP } state;

volatile int STOP = FALSE;

int llopen_rec(int fd) {
  char set[5] = {0x7E, 0x03, 0x03, 0x00, 0x7E};
  char ua[5] = {0x7E, 0x03, 0x07, 0x04, 0x7E};

  int i = 0;
  while (i < 3) {
    char *buf;
    readCycle(fd, buf);
    if (buf[3] == buf[1] ^ buf[2] && buf[2] == 0x03) {
      write(fd, ua, strlen(ua) + 1);
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

int llread(int fd, char *buffer){};

int llclose(int fd){};

int main(int argc, char **argv) {
  int fd;
  struct termios oldtio, newtio;
  char buf[255];

  if ((argc < 2) || ((strcmp("/dev/ttyS0", argv[1]) != 0) &&
                     (strcmp("/dev/ttyS1", argv[1]) != 0))) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  /*
    Open serial port device for reading and writing and not as controlling
    tty because we don't want to get killed if linenoise sends CTRL-C.
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

  newtio.c_cc[VTIME] = 0;
  newtio.c_cc[VMIN] = 1;

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  printf("New termios structure set\n");

  llopen_rec(fd);

  tcsetattr(fd, TCSANOW, &oldtio);
  close(fd);
  return 0;
}
