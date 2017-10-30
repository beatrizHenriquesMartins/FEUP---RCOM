#include "AppLink.h"

unsigned char prevDataCounter = 0;
int dataSize = 100;

/**
 * Stablishing Connection
 * @method connection
 * @param  terminal   0 or 1 for ttyS0 or ttyS1
 * @param  whoCalls   SENDER or RECEIVER
 * @return            file descriptor
 */
int connection(char *terminal, int whoCalls) {
  if (whoCalls != SENDER && whoCalls != RECEIVER) {
    perror("AppLink :: connection() :: Invalid status.");
    return -1;
  }

  application.status = whoCalls;

  int caller = NULL;
  if ((strcmp("0", terminal) == 0 || strcmp("1", terminal) == 0) &&
      whoCalls == SENDER) {
    caller = SENDER;
  } else if ((strcmp("0", terminal) == 0 || strcmp("1", terminal) == 0) &&
             whoCalls == RECEIVER) {
    caller = RECEIVER;
  } else {
    perror("AppLink :: connection() :: terminal failed");
  }

  application.fileDescriptor = llopen(terminal, caller);
  if (application.fileDescriptor < 0) {
    perror("AppLink :: connection() :: llopen failed");
    return -1;
  }
  return application.fileDescriptor;
}

/**
 * Receive Data for receiver
 * @method receiveData
 * @return -1 or 0 in case of success or fail
 */
int receiveData() {
  unsigned char frame[255];
  int over = 0;
  FileInfo file;
  file.size = 0;
  int ret;
  int fp;
  int packagesLost = 0;
  int nRejs = 0;
  int frameSize;

  printf("AppLink :: Start reading\n");

  while (!over) {

    frameSize = llread(application.fileDescriptor, frame);

    if (frameSize == -1) {
      packagesLost++;
      ret = -1;
    } else {
      ret = processingDataPacket(frame, frameSize, &file, fp);

      if (ret == START_CTRL_PACKET) {
        fp = open(file.filename, O_CREAT | O_WRONLY);
        if (fp == -1) {
          printf("AppLink :: Receive Data :: Could not open file %s\n",
                 file.filename);
          return -1;
        }
      }

      if (ret == END_CTRL_PACKET) {
        over = 1;
      }
    }

    if (ret == -1) {
      write(application.fileDescriptor, C_REJ, 5);
      nRejs++;
    } else {
      if (frame[FLD_CTRL] == N_OF_SEQ_0) {
        write(application.fileDescriptor, C_RR1, 5);
      } else
        write(application.fileDescriptor, C_RR, 5);
    }
  }
  // OPEN FILE
  char command[50] = "xdg-open ";
  strcat(command, file.filename);
  system(command);

  printf("\nFile read\n");
  printf("\nPackages lost : %d\n", packagesLost);
  printf("File size : %d\n", file.size);
  printf("Number of rejections sent : %d\n", nRejs);

  return 0;
}

/**
 * Processing Data Packet
 * @method processingDataPacket
 * @param  packet               Packet to Process
 * @param  length               Length of above
 * @param  file                 File to Read
 * @param  fp                   File Descriptor for file
 * @return                      -1 in case of fail or status
 */
int processingDataPacket(unsigned char *packet, int length, FileInfo *file,
                         int fp) {

  int index = 4;
  int numberOfBytes = 0;
  int ret = 0;
  int checkCounterData = 0;

  if (packet[index] == START_CTRL_PACKET || packet[index] == END_CTRL_PACKET) {

    ret = packet[index];
    index += 2;

    numberOfBytes = packet[index];
    index++;
    memcpy(&((*file).size), packet + index, numberOfBytes);

    index += numberOfBytes + 1;

    numberOfBytes = packet[index];
    index++;
    memcpy(&((*file).filename), packet + index, numberOfBytes);

    index += numberOfBytes;

    if (ret == START_CTRL_PACKET) {
      printf("File name : %s\n", file->filename);
      printf("File size : %d\n\n", file->size);
    }
  } else if (packet[index] == DATA_CTRL_PACKET) {


    ret = packet[index];
    index++;
    int counterIndex = index;
    index++;

    unsigned int l2 = packet[index];
    index++;

    unsigned int l1 = packet[index];
    index++;
    unsigned int k = l2*256  + l1;

    unsigned char expect = getBCC2(packet + 4, k + 4);

    if (packet[8 + k] != expect) {
      printf("BCC received: %X\n", packet[8 + k]);
      printf("BCC expected: %X\n", getBCC2(packet + 4, k + 4));
      return -1;
    }

    if (k != (length - 10)) {
      return -1;
    }

    if (prevDataCounter == 0) {
      prevDataCounter = packet[counterIndex];
    } else {
      if (prevDataCounter == packet[counterIndex]) {
        checkCounterData = 1;
        printf("Repeated packet\n");
      } else {
        prevDataCounter = packet[counterIndex];
        checkCounterData = 0;
      }
    }

    int i;
    for (i = 0; i < k; i++) {
      if (checkCounterData == 0)
        write(fp, &packet[index + i], 1);
    }
  }

  return ret;
}

/**
 * Send Control Package
 * @method sendControlPackage
 * @param  state
 * @param  file
 * @param  controlPacket
 * @return                     size of control package
 */
int sendControlPackage(int state, FileInfo file, unsigned char *ctrlPacket) {

  char fileSize[50];

  memcpy(fileSize, &file.size, sizeof(file.size));

  int ctrlPacketSize = 0;

  ctrlPacket[0] = (unsigned char)state;
  ctrlPacket[1] = (unsigned char)0;
  ctrlPacket[2] = (unsigned char)strlen(fileSize);
  ctrlPacketSize = 3;
  unsigned int i;
  for (i = 0; i < strlen(fileSize); i++) {
    ctrlPacket[i + 3] = fileSize[i];
  }
  ctrlPacketSize += strlen(fileSize);

  ctrlPacket[ctrlPacketSize] = (unsigned char)1;
  ctrlPacketSize++;
  ctrlPacket[ctrlPacketSize] = (unsigned char)strlen(file.filename);
  ctrlPacketSize++;

  for (i = 0; i < strlen(file.filename); i++) {
    ctrlPacket[ctrlPacketSize + i] = file.filename[i];
  }
  ctrlPacketSize += strlen(file.filename);

  return ctrlPacketSize;
}

/**
 * Send Data Package
 * @method sendDataPackage
 * @param  dataPacket
 * @param  fp
 * @param  sequenceNumber
 * @param  length
 * @return                  1
 */
int sendDataPackage(unsigned char *dataPacket, FILE *fp, int sequenceNumber,
                    int *length) {

  unsigned char buffer[dataSize];
  int ret;
  ret = fread(buffer, sizeof(char), dataSize, fp);
  if (ret == 0) {
    return 0;
  } else if (ret < 0) {
    return -1;
  }

  *length = ret + 4;

  dataPacket[0] = DATA_CTRL_PACKET;
  dataPacket[1] = sequenceNumber;
  dataPacket[2] = 0;
  dataPacket[3] = ret;

  int j;
  for (j = 0; j < dataSize; j++) {
    dataPacket[4 + j] = buffer[j];
  }

  return 1;
}

/**
 * Send Data for sender
 * @method sendData
 * @return 1
 */
int sendData() {

  char sequenceNumber = N_OF_SEQ_0;
  int dataCounter = 1;
  FileInfo file;
  getFile(file.filename);
  FILE *fp;
  fp = fopen(file.filename, "rb");
  if (fp == NULL) {
    printf("Could not open file  test.c");
    return -1;
  }
  printf("opened file %s\n", file.filename);
  (void)signal(SIGALRM, retry);

  file.size = fileSize(fp);
  printf("File size : %d\n", file.size);

  char fileSize[50];
  memcpy(fileSize, &file.size, sizeof(file.size));

  int packetSize = 5 + strlen(file.filename) + strlen(fileSize);

  unsigned char controlPacket[packetSize];

  int controlPacketSize =
      sendControlPackage(START_CTRL_PACKET, file, controlPacket);

  controlPacket[controlPacketSize] = sequenceNumber;
  controlPacketSize++;

  llwrite(application.fileDescriptor, controlPacket, controlPacketSize);

  int dataPacketSize;
  unsigned char dataPacket[dataSize + 4];
  int ret = 1;
  int nRejs = 0;
  int llwriteRet;

  while (ret != 0) {
    ret = sendDataPackage(dataPacket, fp, dataCounter, &dataPacketSize);

    if (dataCounter < 255)
      dataCounter++;
    else
      dataCounter = 1;
    if (ret != 0) {
      dataPacket[dataPacketSize] = sequenceNumber;
      dataPacketSize++;
      llwrite(application.fileDescriptor, dataPacket, dataPacketSize);
      if (sequenceNumber == N_OF_SEQ_0)
        sequenceNumber = N_OF_SEQ_1;
      else
        sequenceNumber = N_OF_SEQ_0;
    }
  }

  controlPacketSize = sendControlPackage(END_CTRL_PACKET, file, controlPacket);
  controlPacket[controlPacketSize] = sequenceNumber;
  controlPacketSize++;

  llwriteRet =
      llwrite(application.fileDescriptor, controlPacket, controlPacketSize);
  nRejs += llwriteRet;

  printf("\nFile sent\n\n");

  printf("Number of rejections received : %d\n", nRejs);

  return 1;
}

/**
 * Get File Path from command Line
 * @method getFile
 * @param  filepath
 * @return          1
 */
int getFile(char *filepath) {

  printf("Enter file path : ");
  scanf("%s", filepath);

  return 1;
}

/**
 * Get File Size
 * @method fileSize
 * @param  fd       pointer to File
 * @return          size of file
 */
int fileSize(FILE *fd) {
  struct stat s;
  if (fstat(fileno(fd), &s) == -1) {
    return (-1);
  }
  return (s.st_size);
}
