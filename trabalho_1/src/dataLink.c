#include "dataLink.h"

int flag = 1;
int frameSize = 0;
int numberOfTries = 3;
int timeoutTime = 15;
int numberOfTimeOuts = 0;
int success = 0;
int tries = 0;
unsigned char frame[255];
char temp[5];
int fdW;
struct termios oldtio, newtio;

/**
 * Functions dealing with alerts
 */
void retry() {
  alarm(timeoutTime);
  write(fdW, frame, frameSize);
  numberOfTimeOuts++;

  if (tries == numberOfTries) {
    printf(
        "\n\nTIMEOUT : Lost connection to receiver\n Number of tries : %d\n\n",
        numberOfTries);
    exit(1);
  }

  tries++;
  printf("\n\nTrying to connect to receiver\nTry number : %d\n\n", tries);
}

void timeout() {
  printf("TIMEOUT : Connection lost, try again later\n");
  exit(1);
}

// atende alarme
void atende() {
  printf("alarme # %d\n", tries);
  flag = 1;
  tries++;
}

/**
 * Functions
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

  // RECEIVE TRAMA SET
  alarm(timeoutTime);
  char controlByte = readingArrayStatus(fd);
  alarm(0);

  // WRITE TRAMA UA
  char tramaUA[5] = {FLAG, A_SENDER, C_UA, C_UA, FLAG};
  int res;
  do {
    res = write(fd, &tramaUA, sizeof(tramaUA));
  } while (res == 0);

  return fd;
}

int open_sender(char *port) {
  printf("open_sender\n");
  char buffer[5];
  char tramaUA[5];
  int res = 0;
  char controlByte = NULL;

  int fd = open_serial_port(port, SENDER);

  // CREATE AND WRITE TRAMA SET
  createControlFrame(buffer, C_SET, SENDER);
  do {
    res = write(fd, buffer, 5);
    // READ TRAMA UA
    controlByte = readingArrayStatus(fd);
  } while (tries < numberOfTries && flag == 1);

  return fd;
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
  flag = 0;
  alarm(timeoutTime);
  while (state != 5 && flag == 0) {
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
        if (var == (frame_receive[2] ^ frame_receive[1]) ||
            (frame_receive[2] == C_UA && var == frame_receive[2])) {
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
          alarm(0);
          return frame_receive[2];
        }
        break;
      }
      }
    }
  }
  perror("Damage package");
  return -1;
}

void insertValueAt(int index, int value, unsigned char *frame, int length) {
  int i;

  for (i = length - 1; i >= index; i--) {
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

char getBCC2(unsigned char *frame, unsigned int length) {
  unsigned char BCC = 0;

  unsigned int i = 0;
  for (; i < length; i++) {
    BCC ^= frame[i];
  }

  return BCC;
}

int stuffing(unsigned char *frame, int length) {
  int i;
  for (i = 1; i < length - 1; i++) {
    if (frame[i] == FLAG) {
      frame[i] = ESC;
      i++;
      insertValueAt(i, 0, frame, length);
      frameSize++;
      frame[i] = FLAG_HIDE_BYTE;
    }
    if (frame[i] == ESC) {
      i++;
      insertValueAt(i, 0, frame, length);
      frameSize++;
      frame[i] = ESC_HIDE_BYTE;
    }
  }

  return i;
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

int processingDataFrame(char *frame) {
  printf("processingDataFrame\n");
  int ret = 0;

  if (frame[0] != FLAG) {
    return -1;
  }

  if (frame[1] != A_SENDER) {
    return -1;
  }

  if (frame[2] != N_OF_SEQ_0 && frame[2] != N_OF_SEQ_1) {
    return -1;
  }

  if (frame[3] != (frame[1] ^ frame[2])) {
    printf("BCC1 recebido: %X\n", frame[3]);
    printf("BCC1 esperado: %X\n", frame[1] ^ frame[2]);
    printf("ERRO BCC1\n");
    return -1;
  }

  return ret;
}

int readingFrame(int fd, char *frame) {
  unsigned char oneByte;
  int state = 0;
  int over = 0;
  int i = 0;
  int j;

  (void)signal(SIGALRM, timeout);

  while (!over) {
    alarm(timeoutTime);
    read(fd, &oneByte, 1);
    alarm(timeoutTime);

    switch (state) {
    case 0:
      if (oneByte == FLAG) {
        frame[i] = oneByte;
        i++;
        state = 1;
      }
      break;
    case 1:
      if (oneByte != FLAG) {
        frame[i] = oneByte;
        i++;
        state = 2;
      }
      break;
    case 2:
      if (oneByte != FLAG) {
        frame[i] = oneByte;
        i++;
        state = 3;
      }
      break;
    case 3:
      if (oneByte != FLAG) {
        frame[i] = oneByte;
        i++;
        state = 4;
      }
      break;
    case 4:
      if (oneByte != FLAG) {
        frame[i] = oneByte;
        i++;
      } else if (oneByte == FLAG) {
        frame[i] = oneByte;
        i++;
        over = 1;
      }
      break;
    default:
      break;
    }
  }
  return i;
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

int writeTo_tty(int fd, char *buf, int buf_length) {
  int totalWrittenChars = 0;
  int writtenChars = 0;

  while (totalWrittenChars < buf_length) {
    writtenChars = write(fd, buf, buf_length);

    if (writtenChars <= 0) {
      printf("Written chars: %d\n", writtenChars);
      printf("%s\n", strerror(errno));
      return -1;
    }

    totalWrittenChars += writtenChars;
  }

  return 0;
}

int resetSettings(int fd) {
  printf("resetSettings\n");
  if (close(fd)) {
    return -1;
    printf("Error closing terminal file descriptor.\n");
  }
  return 0;
}

int llopen(char *port, int whoCalls) {
  printf("llopen\n");

  if (whoCalls == RECEIVER) {
    return open_receiver(port);
  } else if (whoCalls == SENDER) {
    return open_sender(port);

  } else {
    return -1;
  }
  // return port;
}

int llread(int fd, char *buffer) {
  printf("llread\n");

  int ret, sizeAfterDestuffing;

  readingFrame(fd, buffer);

  sizeAfterDestuffing = destuffing(buffer);

  if (ret == 0) {
    ret = sizeAfterDestuffing;
  }
  return ret;
}

int llwrite(int fd, unsigned char *buffer, int length) {
  printf("Entering llwrite\n");
  fdW = fd;

  int sequenceNumber = buffer[length - 1];
  int nRej = 0;

  length--;
  frame[0] = FLAG;
  frame[1] = A_SENDER;
  frame[2] = sequenceNumber;
  frame[3] = frame[1] ^ frame[2];

  int i;
  for (i = 0; i < length; i++) {
    frame[i + 4] = buffer[i];
  }

  frame[length + 4] = getBCC2(buffer, length);

  tries = 0;
  (void)signal(SIGALRM, retry);

  stuffing(frame, length + 6);
  frame[length + 5] = FLAG;

  i = 0;
  do {
    if (i > 0) {
      nRej++;
    }
    int j;
    alarm(timeoutTime);
    write(fd, frame, sizeof(frame));
    read(fd, temp, 5);
    for (j = 0; j < 5; j++) {
      printf("%x ", temp[j]);
    }
    printf("\n");
    alarm(timeoutTime);
    i++;
  } while (temp[2] == C_REJ);

  return nRej;
}

int llclose(int fd, int whoCalls) {
  printf("llclose\n");

  char *frame = NULL;
  int lenFrame = 0;
  int res_resetSet = 0;

  if (whoCalls == SENDER) {
    createControlFrame(frame, C_DISC, whoCalls);

    if (sendImportantFrame(fd, frame, lenFrame) != 0) {
      printf("Couldn't send frame on llclose().\n");
      res_resetSet = resetSettings(fd);
    }

    createControlFrame(frame, C_UA, &lenFrame);

    if (writeTo_tty(fd, frame, lenFrame) != 0) {
      printf("Couldn't write to tty on llclose()\n");
      res_resetSet = resetSettings(fd);
      return -1;
    }
  } else if (whoCalls == RECEIVER) {
    res_resetSet = resetSettings(fd);
    if (res_resetSet == 0) {
      printf("Connection succesfully closed.\n");
    }
  }
  return 0;
}
