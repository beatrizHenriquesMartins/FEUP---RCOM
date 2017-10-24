#include "dataLink.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int fd;

  if (argc < 2) {
    perror("Invalid num of arguments");
    return -1;
  }

  fd = llopen(argv[1], RECEIVER);
  printf("llopen complete\n");

  char buffer[255];
  llread(fd, buffer);
  printf("llread complete\n");

  // llclose();
  // printf("llclose complete\n");

  return 0;
}
