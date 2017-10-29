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

  fd = llopen(argv[1], SENDER); //= connection(argv[1], SENDER);
  printf("%i\n", fd);
  // sendData(fd);
  if (fd != -1)
    llwrite(fd, "123", strlen("123"));

  return 0;
}
