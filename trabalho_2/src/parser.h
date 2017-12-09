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

#define MAXDATASIZE 512

/*
type struct for url data
*/
typedef struct{
    char * type;
    char * user;
    char * password;
    char * host;
    char * url_path;
} data_url;

/*
function to create a url 
*/
int info_parser(data_url *url_info, char * str);

/*
function to create a name for the file
*/
int parser_filename(char * filename, char path[128]);