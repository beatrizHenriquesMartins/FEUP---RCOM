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

  fd = llopen(argv[1], SENDER);
  printf("llopen complete\n");

  llwrite();
  printf("llwrite complete\n");

  // llclose();
  // printf("llclose complete\n");

  return 0;
}
