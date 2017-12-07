#ifdef __URL_H__
#define __URL_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <fcntl.h>

typedef struct{
    char type[512];
    char user[512];
    char password[512];
    char path[512];
    char host[512];
} url_information;

int parseInfo(char* str, urlInfo* url_info);

int parseFilename(char path[100], char * filename);

#endif