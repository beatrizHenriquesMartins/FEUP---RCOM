#include "AppLink.h"
#include "dataLink.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
  int fd;

  if (argc < 2) {
    perror("Invalid num of arguments");
    return -1;
  }

  connection(argv[1], RECEIVER);
  receiveData();

  return 0;
}
