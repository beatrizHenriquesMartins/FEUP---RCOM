#include "AppLink.h"
#include "dataLink.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int fd;
  char buffer[255];

  if (argc < 2) {
    perror("Invalid num of arguments");
    return -1;
  }

  fd = llopen(argv[1], SENDER);
  printf("%i\n", fd);

  // fd = connection(argv[1], RECEIVER);
  llread(fd, buffer);
  // printf("llread complete!\n");
  // receiveData();

  return 0;
}
