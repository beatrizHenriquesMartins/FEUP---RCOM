#include "AppLink.h"
#include "dataLink.h"
#include <stdio.h>

int main(int argc, char *argv[]) {

  (void)signal(SIGALRM, atende); // instala  rotina que atende interrupcao
  int fd;

  if (argc < 2) {
    perror("Invalid num of arguments");
    return -1;
  }

  fd = connection(argv[1], SENDER);
  printf("llopen complete\n\n");

  if (fd > 0)
    sendData();

  llclose(fd, SENDER);
  printf("llclose complete\n");

  return 0;
}
