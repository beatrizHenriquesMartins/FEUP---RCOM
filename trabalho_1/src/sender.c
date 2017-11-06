#include "AppLink.h"
#include "dataLink.h"
#include <stdio.h>
#include <time.h>

int main(int argc, char *argv[]) {

  struct timeval startT, endT;

  gettimeofday(&startT, NULL);

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

  gettimeofday(&endT, NULL);

  double elapsed = (endT.tv_sec - startT.tv_sec) +
                   ((endT.tv_usec - startT.tv_usec) / 1000000.0);
  printf("%f\n", elapsed);

  return 0;
}
