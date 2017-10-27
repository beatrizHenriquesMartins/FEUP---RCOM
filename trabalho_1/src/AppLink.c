#include "AppLink.h"

int numBytesReads = 0;

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
