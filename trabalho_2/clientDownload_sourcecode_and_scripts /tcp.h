#include "parser.h"

#define CLIENT_CONNECTION_PORT 21
#define MAX_STRING_SIZE 512

int createConnection(char* address, int port);

void login(int controlSocketFD, urlInfo* info);

void passiveMode(int sockfd, char* ip, int* port);

void sendAndRetrieve(int controlSocketFD, urlInfo* info);

int downloadFile(int dataSocketFD, urlInfo* info);

int closeConnection(int controlSocketFD, int dataSocketFD);
