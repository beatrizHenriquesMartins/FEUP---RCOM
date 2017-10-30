#include "dataLink.h"

int flag = 1;
int frameSize = 0;
int nTries = 3;
int outTime = 3;
int nTOuts = 0;
int tries = 0;
unsigned char frame[255];
int fdW;
struct termios oldtio, newtio;

/**
 * Functions dealing with alerts
 */
void retry() {
	alarm(outTime);
	write(fdW, frame, frameSize);
	nTOuts++;

	if (tries == nTries) {
		printf(
				"\n\nTIMEOUT : Lost connection to receiver\n Number of tries : %d\n\n",
				nTries);
		exit(1);
	}

	tries++;
	printf("\n\nTrying to connect to receiver\nTry number : %d\n\n", tries);
}

/**
 * Connection Lost
 */
void timeout() {
	printf("TIMEOUT : Connection lost, try again later\n");
	exit(1);
}

/**
 * atende alarme
 */
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

	if ((strcmp("/dev/ttyS0", serialPort) != 0)
			&& (strcmp("/dev/ttyS1", serialPort) != 0)) {
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

/**
 * Function to Open Receiver and receive trama SET and send trama UA
 * @method open_receiver
 * @param  port          0 or 1 for ttyS0 or ttyS1
 * @return               file descriptor for serial port opened
 */
int open_receiver(char *port) {
	printf("open_receiver\n");
	int fd;

	(void) signal(SIGALRM, timeout);
	fd = open_serial_port(port, RECEIVER);

	// RECEIVE TRAMA SET
	alarm(outTime);
	char controlByte = readingArrayStatus(fd);
	alarm(0);

	// WRITE TRAMA UA
	char tramaUA[5] = { FLAG, A_SENDER, C_UA, C_UA, FLAG };
	int res;
	do {
		res = write(fd, &tramaUA, sizeof(tramaUA));
	} while (res == 0);

	return fd;
}

/**
 * Function to Open Sender and send trama SET and receive trama UA
 * @method open_sender
 * @param  port        0 or 1 for ttyS0 or ttyS1
 * @return             file descriptor for serial port opened
 */
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
                (void) signal(SIGALRM,atende);
		controlByte = readingArrayStatus(fd);
	} while (tries < nTries && flag == 1);

	return fd;
}

/**
 * Create Control Frame SET, DISC, etc
 * @method createControlFrame
 * @param  frame              pointer to frame to create
 * @param  controlByte        type of frame
 * @param  whoCalls           who calls the function: SENDER or RECEIVER
 */
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

/**
 * Status Machine for reading Array Status
 * @method readingArrayStatus
 * @param  fd                 file descriptor
 * @return                    Control Camp
 */
char readingArrayStatus(int fd) {
	int state = 0;
	unsigned char frame_receive[5];
	unsigned char var;
	flag = 0;
	alarm(outTime);
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
					printf("Damage package\n");
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
}

/**
 * Shift forward method in which we insert value on frame at index
 * @method insertValueAt
 * @param  index
 * @param  value
 * @param  frame
 * @param  length
 */
void insertValueAt(int index, int value, unsigned char *frame, int length) {
	int i;

	for (i = length - 1; i >= index; i--) {
		frame[i + 1] = frame[i];
	}

	frame[i] = value;
}

/**
 * Shift Back Method
 * @method shiftBack
 * @param  index
 * @param  frame
 * @param  frameSize
 */
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

/**
 * Get Block Check Character 2 of Frame
 * @method getBCC2
 * @param  frame
 * @param  length
 * @return  BCC2
 */
unsigned char getBCC2(unsigned char *frame, unsigned int length) {
	unsigned char BCC = 0;

	unsigned int i = 0;
	for (; i < length; i++) {
		BCC ^= frame[i];
	}

	return BCC;
}

/**
 * Stuffing of frame
 * @method stuffing
 * @param  frame
 * @param  length
 * @return          size of frame after stuffing
 */
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

/**
 * Destuffing of frame
 * @method destuffing
 * @param  frame
 * @return            size of frame after destuffing
 */
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

/**
 * Processing Data Frame
 * @method processingDataFrame
 * @param  frame
 * @return                     0 or -1 in case of success or fail
 */
int processingDataFrame(char *frame) {
	printf("processingDataFrame\n");

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

	return 0;
}

/**
 * Reading Frame for llread
 * @method readingFrame
 * @param  fd           file descriptor for read
 * @param  frame
 * @return              size of frame read
 */
int readingFrame(int fd, char *frame) {
	unsigned char oneByte;
	int state = 0;
	int over = 0;
	int i = 0;
	int j;

	(void) signal(SIGALRM, timeout);

	while (!over) {
                alarm(outTime);
		read(fd, &oneByte, 1);
                alarm(outTime);

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

/**
 * Reset Settings and Close File Descriptor
 * @method resetSettings
 * @param  fd
 * @return               0 or -1 in case of success or fail
 */
int resetSettings(int fd) {
	printf("resetSettings\n");
	if (close(fd)) {
		return -1;
		printf("Error closing terminal file descriptor.\n");
	}
	return 0;
}

/**
 * LLOPEN
 * @method llopen
 * @param  port      0 or 1 for ttyS0 or ttyS1
 * @param  whoCalls  SENDER or RECEIVER
 * @return           file descriptor
 */
int llopen(char *port, int whoCalls) {
	printf("\nllopen\n");

	if (whoCalls == RECEIVER) {
		return open_receiver(port);
	} else if (whoCalls == SENDER) {
		return open_sender(port);

	} else {
		return -1;
	}
	// return port;
}

/**
 * LLREAD
 * @method llread
 * @param  fd
 * @param  buffer
 * @return        size after destuffing
 */
int llread(int fd, char *buffer) {

	int ret, sizeAfterDestuffing;

	readingFrame(fd, buffer);

	sizeAfterDestuffing = destuffing(buffer);

	if (ret == 0) {
		ret = sizeAfterDestuffing;
	}
	return ret;
}

/**
 * LLWRITE
 * @method llwrite
 * @param  fd
 * @param  buffer
 * @param  length
 * @return         number of rejections
 */
int llwrite(int fd, unsigned char *buffer, int length) {
        printf("tries: %d\n",tries);
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
	(void) signal(SIGALRM, retry);

	stuffing(frame, length + 6);
	frame[length + 5] = FLAG;

	unsigned char temp[5];
	i = 0;
	do {
		if (i > 0) {
			nRej++;
		}
		int j;
		alarm(outTime);
		write(fd, frame, sizeof(frame));
		read(fd, temp, 5);
		alarm(outTime);
		i++;
	} while (temp[2] == C_REJ);

	return nRej;
}

/**
 * LLCLOSE
 * @method llclose
 * @param  fd
 * @param  whoCalls SENDER or RECEIVER
 * @return          0 or -1 in case of success or fail
 */
int llclose(int fd, int whoCalls) {
	printf("\nllclose\n");

	char frame[5];
	int res_resetSet = 0, res = 0;

	tries = 0;
	(void) signal(SIGALRM, atende);

	if (whoCalls == SENDER) {
		createControlFrame(frame, C_DISC, whoCalls);
		if ((res = write(fd, &frame, sizeof(frame))) != 5) {
			printf("llclose :: Couldn't send frame DISC on llclose().\n");
			res_resetSet = resetSettings(fd);
			return -1;
		}

		alarm(outTime);
		if (readingArrayStatus(fd)) {
			alarm(outTime);
			tries = 0;
		}

		char tramaUA[5] = { FLAG, A_SENDER, C_UA, C_UA ^ A_SENDER, FLAG };
		if (res = write(fd, &tramaUA, sizeof(tramaUA)) != 5) {
			printf("llclose :: Couldn't send frame UA on llclose().\n");
			res_resetSet = resetSettings(fd);
			return -1;
		}
        sleep(1);

		res_resetSet = resetSettings(fd);
		if (res_resetSet == 0) {
			printf("llclose :: Connection successfully closed.\n");
		}
	} else if (whoCalls == RECEIVER) {

		alarm(outTime);
		res = 0;
		if ((res = readingArrayStatus(fd)) != -1) {
			alarm(outTime);
			tries = 0;
		}
         sleep(1);

		createControlFrame(frame, C_DISC, whoCalls);
		if ((res = write(fd, frame, sizeof(frame))) != 5) {
			printf("llclose :: Couldn't send frame DISC on llclose().\n");
			res_resetSet = resetSettings(fd);
			return -1;
		}
		res = 0;
		alarm(outTime);
		if ((res = readingArrayStatus(fd)) != -1) {
			printf("UA %d\n",res);
			alarm(outTime);
			tries = 0;
		}

		res_resetSet = resetSettings(fd);
		if (res_resetSet == 0) {
			printf("llclose :: Connection successfully closed.\n");
		}
	}

	printf("Number of timeouts : %d\n", nTOuts);

	return 0;
}


