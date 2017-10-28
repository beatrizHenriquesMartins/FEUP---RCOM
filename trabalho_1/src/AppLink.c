#include "AppLink.h"

int numBytesReads = 0;

/*
size_t - size of objs (C++)
off_t - file objs (POSIX)
*/
off_t getFileSize(char* trama, int lenghtTrama) {
  int i = 1;
  while (i < lenghtTrama) {
    if (trama[i] == FILE_SIZE_BYTE){
      //necessário fazer conversão de char* to long long
      return *((off_t *)(trama + i + 2));
    }
    i += 2 + trama[i + 1];
  }
  return 0;
}

char* getFileName(char* trama, int lenghtTrama) {
  int i = 1;
  while (i < lenghtTrama) {
    if (trama[i] == FILE_NAME_BYTE) {
      char *file_name = (char*)malloc((trama[i + 1] + 1) * sizeof(char));
      //copia da trama para file_name n caracteres
      memcpy(file_name, trama + i + 2, trama[i + 1]);
      file_name[(trama[i + 1] + 1)] = 0;
      return file_name;
    }
    i += 2 + trama[i + 1];
  }
  return NULL;
}

/*
mode_t tem 4 bytes, no entanto
a trama enviada tem apenas 2, por isso é necessário
ler apenas 2 e fazer cast
*/
mode_t getFileMode(char* trama, int lenghtTrama){
  int i = 1;

  while (i < lenghtTrama) {
    if (trama[i] == FILE_PERMISSIONS_BYTE) {
      return *((mode_t*)(trama + i + 2));
    }

    i += 2 + trama[i+2];
  }

  return -1;
}

int connection(char* terminal,int whoCalls){
  if(whoCalls != SENDER && whoCalls != RECEIVER){
    perror("AppLink :: connection() :: Invalid status.");
    return -1;
  }

  application.status = whoCalls;

  int port = NULL ;
  if(strcmp("/dev/ttyS0", terminal) == 0){
    port = SENDER;
  }else if(strcmp("/dev/ttyS1", terminal) == 0){
    port = RECEIVER;
  }else{
    perror("AppLink :: connection() :: terminal failed");
  }

  application.fileDescriptor = llopen(terminal, port);
  if(application.fileDescriptor < 0){
    perror("AppLink :: connection() :: llopen failed");
    return -1;
  }
  return application.fileDescriptor;
}

int receiveData(){
  char trama[PACKET_SIZE];

  int lenghtTrama = 0;
  int llread_res = 0;
  int appFD = 0;

  do {
    appFD = application.fileDescriptor;
    llread_res = llread(appFD, trama);
    if ( llread_res != 0) {
      perror("Error llread() :: receive()");
      exit(-1);
    }
  } while (lenghtTrama == 0 || trama[0] != (unsigned char)START_BYTE);

  //tipos correctos de obter dados
  off_t fSize = getFileSize(trama, lenghtTrama);
  char* fName = getFileName(trama, lenghtTrama);
  mode_t fMode = getFileMode(trama, lenghtTrama);

  int fd = open(fName, O_WRONLY | O_CREAT | O_TRUNC);

  if (fd < 0) {
    perror("Error opening file. Exiting...");
    return -1;
  }

  /*reading trama*/
  char sequenceNumber = 0;
  appFD = application.fileDescriptor;
  llread_res = llread(appFD, trama);
  if (llread_res != 0) {
    perror("Error llread() :: receive");
    close(fd);
    exit(-1);
  }

  while (lenghtTrama == 0 || trama[0] != END_BYTE) {
    if (lenghtTrama > 0 && sequenceNumber == trama[1]) {
      int lenghtInfo = trama[2] * 256 + trama[3];

      write(fd, trama + 4, lenghtInfo);

      numBytesReads += lenghtInfo;
      sequenceNumber++;
    }

    if(llread_res != 0){
      perror("Error llread() :: receive");
      close(fd);
      exit(-1);
    }
  }

  close(fd);

  return 0;
}
