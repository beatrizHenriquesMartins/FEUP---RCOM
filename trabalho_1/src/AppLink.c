#include "AppLink.h"

int numBytesReads = 0;

/* ??? */
int getFileSize(char *trama, int lenghtTrama) {
  int i = 1;
  while (i < lenghtTrama) {
    if (trama[i] == FILE_SIZE_BYTE){
      return (packet + i + 2));
    }
    i += 2 + packet[i + 1];
  }

  return 0;
}

char *getFileName(char *packet, int packet_len) {
  int i = 1;
  while (i < packet_len) {
    if (packet[i] == FILE_NAME_BYTE) {
      char *file_name = (char *)malloc((packet[i + 1] + 1) * sizeof(char));
      memcpy(file_name, packet + i + 2, packet[i + 1]);
      file_name[(packet[i + 1] + 1)] = 0;
      return file_name;
    }

    i += 2 + packet[i + 1];
  }

  return NULL;
}

int connection(char*terminal,int whoCalls){
  if(whoCalls != SENDER && whoCalls != RECEIVER){
    printf("AppLink :: connection() :: Invalid status.\n");
    return -1;
  }

  application.status = whoCalls;

  int port;
  if(strcmp("/dev/ttyS0", terminal) == 0){
    port = SENDER;
  }else if(strcmp("/dev/ttyS1", terminal) == 0){
    port = RECEIVER;
  }

  application.fileDescriptor = llopen(port,whoCalls);
  if(application.fileDescriptor < 0){
    printf("AppLink :: connection() :: llopen failed\n\n");
    return -1;
  }

  return application.fileDescriptor;
}

int receiveData(){
  char trama[PACKET_SIZE];

  int lenghtTrama;

  do {
    if (llread(application.fileDescriptor, trama) != 0) {
      printf("Error llread() :: receive_data()\n");
      exit(-1);
    }
  } while (lenghtTrama == 0 || trama[0] != (unsigned char)START_BYTE);

  int fSize = getFileSize(trama, lenghtTrama);
  char* fName = getFileName(trama, lenghtTrama);
  // file mode

  int fd = open(fName, O_WRONLY | O_CREAT | O_TRUNC);

  if (fd < 0) {
    printf("Error opening file. Exiting...\n");
    return -1;
  }

  /*ainda inacabado*/
  return 0;
}
