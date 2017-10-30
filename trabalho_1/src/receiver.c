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

  fd = connection(argv[1], RECEIVER);
  printf("llopen complete\n\n");

  if (fd > 0)
    receiveData();

  llclose(fd, RECEIVER);
  printf("llclose complete\n");

  return 0;
}
