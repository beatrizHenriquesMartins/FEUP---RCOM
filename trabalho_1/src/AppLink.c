#include "AppLink.h"

int numBytesReads = 0;

int test_connection(char*terminal,int whoCalls){
  if(whoCalls != SENDER && whoCalls != RECEIVER){
    printf("AppLink :: test_connection() :: Invalid status.\n");
    return -1;
  }

  application.status = whoCalls;

  int port;
  if(strcmp("/dev/ttyS0", terminal) == 0){
    port = SENDER;
  }else if(strcmp("/dev/ttyS1", serialPort) == 0){
    port = RECEIVER;
  }
}
