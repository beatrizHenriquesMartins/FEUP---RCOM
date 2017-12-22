#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tcp.h"

#define READ 1
#define NO_READ 0

/**
 * Read From Socket
 * @method readSocket
 * @param  sockfd     socket's file descriptor
 * @param  str        to read
 * @return
 */
int readSocket(int sockfd, char *str) {
  FILE *fp = fdopen(sockfd, "r");
  int allocated = 0;
  if (str == NULL) {
    str = (char *)malloc(sizeof(char) * MAX_STRING_SIZE);
    allocated = 1;
  }
  do {
    memset(str, 0, MAX_STRING_SIZE);
    str = fgets(str, MAX_STRING_SIZE, fp);
    printf("%s", str);
  } while (!('1' <= str[0] && str[0] <= '5') || str[3] != ' ');
  char reply_series = str[0];
  if (allocated)
    free(str);
  return (reply_series > '4');
}

/**
 * Write to Socket
 * @method writeSocket
 * @param  sockfd      socket's file descriptor
 * @param  cmd         command to write
 * @param  response    response from read
 * @param  read
 * @return
 */
int writeSocket(int sockfd, char *cmd, char *response, int read) {
  int return_val = write(sockfd, cmd, strlen(cmd));
  if (read)
    return readSocket(sockfd, response);
  else
    return (return_val == 0);
}

/**
 * creat connection to address
 * original code from the clientTCP.c file given on moodle
 * @method createConnection
 * @param  address
 * @param  port
 * @return
 */
int createConnection(char *address, int port) {
  int sockfd;
  struct sockaddr_in server_addr;

  /*handling server address (part of clientTCP original code)*/
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr =
      inet_addr(address); /*32 bit Internet address network byte ordered*/
  server_addr.sin_port =
      htons(port); /*server TCP port must be network byte ordered */

  /*open TCP socket*/
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket()");
    return 0;
  }
  /*connect to server*/
  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("connect()");
    return 0;
  }
  return sockfd;
}

/**
 * Login to user
 * @method login
 * @param  controlSocketFD socket's file descriptor
 * @param  info            info of user to login
 */
void login(int controlSocketFD, urlInfo *info) {

  char username_cmd[MAX_STRING_SIZE], password_cmd[MAX_STRING_SIZE];

  readSocket(controlSocketFD, NULL);

  sprintf(username_cmd, "USER %s\r\n", info->user);
  writeSocket(controlSocketFD, username_cmd, NULL, READ);
  sprintf(password_cmd, "PASS %s\r\n", info->password);
  if (writeSocket(controlSocketFD, password_cmd, NULL, READ) != 0) {
    fprintf(stderr, "Bad login. Exiting...\n"); // TODO: Ask for valid login
    exit(1);
  }
}

/**
 * Enter Passive Mode
 * @method passiveMode
 * @param  sockfd      socket's file descriptor
 * @param  ip
 * @param  port
 */
void passiveMode(int sockfd, char *ip, int *port) {
  char response[MAX_STRING_SIZE];

  if (writeSocket(sockfd, "PASV\r\n", response, READ) != 0) {
    fprintf(stderr, "Error entering passive mode. Exiting...\n");
    exit(1);
  }

  int values[6];
  char *data = strchr(response, '(');
  sscanf(data, "(%d, %d, %d, %d, %d, %d)", &values[0], &values[1], &values[2],
         &values[3], &values[4], &values[5]);
  sprintf(ip, "%d.%d.%d.%d", values[0], values[1], values[2], values[3]);
  *port = values[4] * 256 + values[5];
}

/**
 * Send command to retrieve Data from file to download
 * @method sendAndRetrieve
 * @param  controlSocketFD control socket's file descriptor
 * @param  info            link's info
 */
void sendAndRetrieve(int controlSocketFD, urlInfo *info) {
  char cmd[MAX_STRING_SIZE];

  writeSocket(controlSocketFD, "TYPE L 8\r\n", NULL, READ);
  sprintf(cmd, "RETR %s%s\r\n", info->path, info->filename);
  if (writeSocket(controlSocketFD, cmd, NULL, READ) != 0) {
    fprintf(stderr, "Error retrieving file. Exiting...\n");
    exit(1);
  }
}

/**
 * Download File
 * @method downloadFile
 * @param  dataSocketFD data socket's file descriptor
 * @param  info         link's info
 * @return
 */
int downloadFile(int dataSocketFD, urlInfo *info) {
  FILE *fileToDownload;
  if (!(fileToDownload = fopen(info->filename, "w"))) {
    printf("ERROR: Cannot open file.\n");
    return 1;
  }

  char buffer[1024];
  int sizeInBytes;
  while ((sizeInBytes = read(dataSocketFD, buffer, sizeof(buffer)))) {
    if (sizeInBytes < 0) {
      fprintf(stderr, "ERROR: Nothing was received from data socket fd.\n");
      return 1;
    }
    if ((sizeInBytes = fwrite(buffer, sizeInBytes, 1, fileToDownload)) < 0) {
      fprintf(stderr, "ERROR: Cannot write data in file.\n");
      return 1;
    }
  }

  fclose(fileToDownload);
  printf("Finished file's download!\n");

  return 0;
}

/**
 * Close Connection
 * @method closeConnection
 * @param  controlSocketFD control socket's file descriptor
 * @param  dataSocketFD    data socket's file descriptor
 * @return
 */
int closeConnection(int controlSocketFD, int dataSocketFD) {

  printf("Closing connection\n");
  if (writeSocket(controlSocketFD, "QUIT\r\n", NULL, NO_READ) != 0) {
    fprintf(stderr, "Error closing connection. Exiting anyway...\n");
    close(dataSocketFD);
    close(controlSocketFD);
    exit(1);
  }

  close(dataSocketFD);
  close(controlSocketFD);

  printf("End of Program!\n");

  return 0;
}
