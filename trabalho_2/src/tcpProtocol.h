#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "parser.h"

#define BUFF_SIZE 512

int write_socket(int sockfd, const char *message);

int read_socket(int sockfd, char * answer);

int login(int sockfd, data_url *url_info);

int set_passive_mode(int sockfd, char *answer);

int connection(int socket, struct  addrinfo *res);

int get_ip(char *answer_ip, char ip[128], char port1[128], char port2[128]);

int get_string(char *info, int numberC, char ret[128]);

int asking_file(int socketfd, data_url *url_info);

int read_file(int data_fd,char *fileName);