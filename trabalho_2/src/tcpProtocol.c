#include "tcpProtocol.h"

int read_socket(int sockfd, char* str){
    FILE* fp = fdopen(sockfd, "r");
    int allocated = 0;
    
    if(str == NULL){
        str = (char*) malloc(sizeof(char) * MAXDATASIZE);
        allocated = 1;
    }
        
    do {
        memset(str, 0, MAXDATASIZE);
        str = fgets(str, MAXDATASIZE, fp);
        printf("%s", str);
    } while (!('1' <= str[0] && str[0] <= '5') || str[3] != ' ');
  
    char reply_series = str[0];
  
    if(allocated){
        free(str);
    }
  
    return (reply_series > '4');
}

int write_socket(int sockfd, char* cmd, char* response, int read){
    int return_val = write(sockfd, cmd, strlen(cmd));
    
    if(read){
      return read_socket(sockfd, response);
    }else{
        return (return_val == 0);
    }
}

void login(int control_socket_fd, data_url* info){
    char username_cmd[MAXDATASIZE], password_cmd[MAXDATASIZE];

    read_socket(control_socket_fd, NULL);

    sprintf(username_cmd, "USER %s\r\n", info->user);

    write_socket(control_socket_fd, username_cmd, NULL, READ);
    
    sprintf(password_cmd, "PASS %s\r\n", info->password);
    
    if(write_socket(control_socket_fd, password_cmd, NULL, READ) != 0){
        fprintf(stderr, "Bad login. Exiting...\n");
        exit(1);
    }
}