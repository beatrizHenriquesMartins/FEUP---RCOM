#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "tcp.h"

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <address>\n", argv[0]);
    exit(1);
  }

  urlInfo info;
  if (parseURL(argv[1], &info) != 0) {
    fprintf(stderr, "Invalid URL\n");
    exit(1);
  }

  int controlSocketFD;
  if ((controlSocketFD = createConnection(
           inet_ntoa(*((struct in_addr *)info.infoHost->h_addr)),
           CLIENT_CONNECTION_PORT)) == 0) {
    fprintf(stderr, "Error opening control connection\n");
    exit(1);
  }

  login(controlSocketFD, &info);
  char data_address[MAX_STRING_SIZE];
  int port;
  passiveMode(controlSocketFD, data_address, &port);

  int dataSocketFD;
  if ((dataSocketFD = createConnection(data_address, port)) == 0) {
    fprintf(stderr, "Error opening data connection\n");
    exit(1);
  }
  sendAndRetrieve(controlSocketFD, &info);
  downloadFile(dataSocketFD, &info);
  closeConnection(controlSocketFD, dataSocketFD);

  return 0;
}
