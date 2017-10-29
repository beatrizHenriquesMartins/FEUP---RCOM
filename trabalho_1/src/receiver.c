#include "AppLink.h"
#include "dataLink.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int fd;

  if (argc < 2) {
    perror("Invalid num of arguments");
    return -1;
  }

  fd = connection(argv[1], RECEIVER);
  printf("llopen complete\n");

  receiveData();

  // llclose();
  // printf("llclose complete\n");

  return 0;
}
