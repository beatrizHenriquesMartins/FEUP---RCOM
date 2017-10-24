#include <stdio.h>
#include "dataLink.h"

int main(int argc, char *argv[]){
  int fd;

  if (argc < 2){
    perror("Invalid num of arguments");
    return -1;
  }

  fd = llopen(argv[1], RECEIVER);

  printf("llopen complete\n");

  return 0;
}
