#include "AppLink.h"
#include "dataLink.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  printf("sender main\n");
  (void)signal(SIGALRM, atende); // instala  rotina que atende interrupcao
  int fd;

  if (argc < 2) {
    perror("Invalid num of arguments");
    return -1;
  }

  fd = connection(argv[1], SENDER);
  printf("llopen complete\n");

  char buffer[255];
  llwrite(fd, buffer, 255);
  printf("llwrite complete\n");

  // llclose();
  // printf("llclose complete\n");

  return 0;
}
