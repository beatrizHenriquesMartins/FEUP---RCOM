#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "parser.h"

#define READ 1
#define NO_READ 0

/*
read from socket
*/
int read_socket(int sockfd, char* str);

/*
write to socket
*/
int write_socket(int sockfd, char* cmd, char* response, int read);

/*
login into server
*/
void login(int control_socket_fd, data_url* info)