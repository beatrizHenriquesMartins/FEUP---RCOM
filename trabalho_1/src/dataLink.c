#include "dataLink.h"

int frameSize = 0;
int numberOfTries = 3;
int timeoutTime = 3;
int numberOfTimeOuts = 0;
int success = 0;
int tries = 0;
struct termios oldtio, newtio;

/*void retry() {
  alarm(timeoutTime);
  write(fd, frame, frameSize);
  numberOfTimeOuts++;

  if (triesPackets == numberOfTries) {
    printf(
        "\n\nTIMEOUT : Lost connection to receiver\n Number of tries : %d\n\n",
        numberOfTries);
    exit(1);
  }

  triesPackets++;
  printf("\n\nTrying to connect to receiver\nTry number : %d\n\n",
         triesPackets);
}
*/
void timeout() {
  printf("TIMEOUT : Connection lost, try again later\n");
  exit(1);
}

/**
 *
 * Funções
 *
 */
int open_serial_port(char *port, int whoCalls) {
  printf("open_serial_port\n");
  int fd;
  char serialPort[10] = "/dev/ttyS";
  strcat(serialPort, port);

  if ((strcmp("/dev/ttyS0", serialPort) != 0) &&
      (strcmp("/dev/ttyS1", serialPort) != 0)) {
    printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
    exit(1);
  }

  fd = open(serialPort, O_RDWR | O_NOCTTY);

  if (fd < 0) {
    perror(serialPort);
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

  if (whoCalls == SENDER) {
    newtio.c_cc[VTIME] = 1;
    newtio.c_cc[VMIN] = 0;
  } else if (whoCalls == RECEIVER) {
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN] = 1;
  }

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1) {
    perror("tcsetattr");
    exit(-1);
  }

  return fd;
}

int open_receiver(char *port) {
  printf("open_receiver\n");
  int fd;

  (void)signal(SIGALRM, timeout);
  fd = open_serial_port(port, RECEIVER);

  alarm(timeoutTime);
  char controlByte = readingArrayStatus(fd);
  alarm(0);

  int res;
  do{
    res = write(fd,&controlByte,sizeof(controlByte));
  }while (res == 0);

  return fd;
}

int open_sender(char *port) {
  char *set_frame = NULL;

  int fd = open_serial_port(port, SENDER);

  createControlFrame(set_frame, C_SET, SENDER);

  sendImportantFrame(fd, set_frame, 5);

  return fd;
}

int sendImportantFrame(int fd, char *frame, int length) {
  printf("sttufing\n");
  stuffing(frame, &length);
  int res;
  tries = 0;
  do {
    res = write(fd, frame, length);
    sleep(1);
    tries++;
  } while (res < 0 && tries < numberOfTries);
  if (tries == numberOfTries) {
    perror("Can not write to serial port");
    return -1;
  }
  return 0;
}

void insertValueAt(int index, int value, char *frame, int lenght) {
  int i;
  for (i = lenght - 1; i >= index; i--) {
    frame[i + 1] = frame[i];
  }
  frame[i] = value;
}

void shiftBack(int index, char *frame, int frameSize) {
  int over = 0;
  index++;
  do {
    frame[index] = frame[index + 1];
    index++;
    if (frame[index] == FLAG) {
      over = 1;
    }
  } while (!over);
}

void stuffing(char *frame, int *length) {
  int i;
  printf("dentro do stuff, len = %d\n", *length);
  int allocated_space = *length;
  for (i = 0; i < *length; i++) {
    printf("dentro ciclo\n");
    printf("frame[0]= 0x%02X\n", frame[0]);
    if (frame[i] == ESC || frame[i] == FLAG) {

      if ((*length) >= allocated_space) {
        allocated_space = 2 * (*length);
        printf("antes realloc\n");
        frame = realloc(frame, allocated_space);
        printf("depois do realloc\n");
      }
      frame[i] = frame[i] ^ 0x20;
      insertValueAt(i, ESC, frame, *length);
      (*length)++;
      i++;
    }
  }
}

int destuffing(char *frame) {
  int over = 0;

  int i = 1;
  while (!over) {
    if (frame[i] == FLAG) {
      over = 1;
    } else if (frame[i] == ESC && frame[i + 1] == FLAG_HIDE_BYTE) {
      frame[i] = FLAG;
      shiftBack(i, frame, 0);
    } else if (frame[i] == ESC && frame[i + 1] == ESC_HIDE_BYTE) {
      shiftBack(i, frame, 0);
    }
    i++;
  }
  return i;
}

void createControlFrame(char *frame, char controlByte, int whoCalls) {
  int isAnswer = 0;
  frame[0] = FLAG;

  // verificação resposta ou cmd
  if (controlByte == C_UA || controlByte == C_RR || controlByte == C_REJ) {
    isAnswer = 1;
  }

  if (whoCalls == SENDER) {
    (isAnswer) ? (frame[1] = A_RECEIVER) : (frame[1] = A_SENDER);
  } else {
    (isAnswer) ? (frame[1] = A_SENDER) : (frame[1] = A_RECEIVER);
  }

  frame[2] = controlByte;
  frame[3] = frame[1] ^ frame[2];
  frame[4] = FLAG;
}

char readingArrayStatus(int fd) {
  int state = 0;
  char frame_receive[5];
  char var;

  while (state != 5) {
    int res = read(fd, &var, 1);
    frame_receive[state] = var;
    if (res > 0) {
      switch (state) {
      case 0: {
        if (var == FLAG) {
          state = 1;
        }
        break;
      }
      case 1: {
        if (var != FLAG) {
          state = 2;
        } else {
          state = 1;
        }
        break;
      }
      case 2: {
        if (var != FLAG) {
          state = 3;
        } else {
          state = 1;
        }
        break;
      }
      case 3: {
        if (var == (frame_receive[2] ^ frame_receive[1])) {
          state = 4;
        } else {
          perror("Damage package");
          return -1;
        }
        break;
      }
      case 4: {
        if (var != FLAG) {
          state = 0;
        } else {
          state = 5;
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

int resetSettings(int fd) {
/*
  if (sigaction(SIGALRM, &data_link.old_action, NULL) == -1)
    printf("Error setting SIGALRM handler to original.\n");

  if (tcsetattr(fd, TCSANOW, &old_port_settings) == -1)
    printf("Error settings old port settings.\n");
*/
  if (close(fd)) {
    return -1;
    printf("Error closing terminal file descriptor.\n");
  }

  return 0;
}

int llopen(char* port, int whoCalls){
  if (whoCalls == RECEIVER){
    open_receiver(port);
  } else if (whoCalls == SENDER) {
    open_sender(port);
  } else {
    return -1;
  }
  return 0;
}

int llclose(int fd, int whoCalls){
  char* frame = NULL;
  int lenFrame = 0;

  if(whoCalls == SENDER){
    createControlFrame(frame, C_DISC, whoCalls);
    if(sendImportantFrame(fd, frame, lenFrame) != 0){
      printf("Couldn't send frame on llclose().\n");
      resetSettings(fd);
    }
  }else if (whoCalls == RECEIVER){

  }

  return 0;
}
