#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int llopen(int porta, int flag) {

  int try
    = 0;
  int connected = 0;

  switch (flag) {
  case TRANSMITTER:
    while (!connected) {
      if (try == 0)
    }
    break;
  case RECEIVER:
    break;
  }
}
