#include "tcpProtocol.h"

int write_socket(int sockfd, const char *message){
    int message_len = 0;
    message_len = write(sockfd, message, strlen(message));

    /*
    if (message_len == strlen(message)){
        return 1;
    }
    */

    printf("Writting to server... \n\n");
    
    return 0;
}

int read_socket(int sockfd, char * answer){
    int answer_len = 0;
    
    // receive a message from a socket
    // recv(int socket, void *buffer, size_t length, int flags)
    answer_len = recv(sockfd, answer, MAX_SIZE - 1, 0);

    if (answer_len == -1) {
        printf("Receiving data ...\n\n");
        exit(1);
    }

    answer[answer_len] = '\0';

    return 0;
}

int login(int sockfd, data_url *url_info){
    //create var
    char password_cmd[MAX_SIZE];
    char login_cmd[MAX_SIZE];
    
    //saving memory
    memset(login_cmd, 0, sizeof(login_cmd));
    memset(password_cmd, 0, sizeof(password_cmd));
    
    //create a user
    //concatenate strings
    //strcat(char *restrict s1, const char *restrict s2);
    strcat(login_cmd, "USER ");
    strcat(login_cmd, url_info->user);
    strcat(login_cmd, "\r\n");
    printf("%s\n", login_cmd);

    //testing user
    write_socket(sockfd, login_cmd);
    char answer_user[MAX_SIZE];
    read_socket(sockfd, answer_user);
    printf("%s\n",answer_user);

    //create a password for user
    strcat(password_cmd, "PASS ");
    strcat(password_cmd, url_info->password);
    strcat(password_cmd, "\r\n");
    printf("PASS: %s\n",password_cmd );

    //testing password
    write_socket(sockfd, password_cmd);
    char answer_pass[MAX_SIZE];
    read_socket(sockfd, answer_pass);
    printf("%s\n",answer_pass);

    return 0;
}

int set_passive_mode(int sockfd, char *answer){
    char pasv_cmd[9]="PASV \r\n";

    write_socket(sockfd, pasv_cmd);
    read_socket(sockfd, answer);

    return 0;
}

int connection(int socket, struct  addrinfo *res){
    int anw_connection = connect(socket, res->ai_addr, res->ai_addrlen);
    if ( anw_connection < 0) {
        printf("connect()\n\n");
        exit(0);
    }
    
    return 1;
}

int get_ip(char *answer_ip, char ip[128], char port1[128], char port2[128]){
    char *ans  = calloc(MAX_SIZE, sizeof(char));;
    ans = strchr(answer_ip, '(');
    ans++;

    get_string(ans, 4, ip);
    printf("11111111\n" );
    ans += strlen(ip);
    get_string(ans, 2, port1);
    ans += strlen(port1);
    get_string(ans, 2, port2);

    memmove(port1, port1+1, strlen(port1));
    memmove(port2, port2+1, strlen(port2));

    return 0;
}

int get_string(char *info, int numberC, char ret[128]){
    int i = 0;
    int count_numberC = 0;
    
    while(1){
        if(*info == ',' || *info == ')'){
            count_numberC++;

            if (count_numberC == numberC){
                break;
            }else{
                ret[i] = '.';
            }
        }else{
            ret[i] = *info;
        }

        i++;
        info++;
    }

    ret[i] = '\0';

    return 1;
}

int asking_file(int socketfd, data_url *url_data){
    char file_path_download_cmd[MAX_SIZE];
    memset(file_path_download_cmd,0,sizeof(file_path_download_cmd));

    strcat(file_path_download_cmd, "RETR ");
    strcat(file_path_download_cmd, url_data->url_path);
    strcat(file_path_download_cmd, "\r\n");

    ssize_t aux_write_socket = write(socketfd, file_path_download_cmd, strlen(file_path_download_cmd));

    if( aux_write_socket < 0){
        perror("error on write retrive command to the server");
        exit(-1);
    }

    char ans[MAX_SIZE];
    read_socket(socketfd, ans);

    printf("%s\n",ans);

    char aux[5];
    aux[0] = ans[0];
    aux[1] = ans[1];
    aux[2] = ans[2];
    aux[3] = '\0';

    int aux_int = atoi(aux);
    if(aux_int == 550){
        printf("File does not exit!Ending program!\n");
        exit(-1);
    }

    return 0;
}

int read_file(int data_fd,char fileName[MAX_SIZE]){
    int new_file_fd = open(fileName, O_CREAT|O_WRONLY, 0777);
    int ans;
    char buf[MAX_SIZE];

    if(new_file_fd < 0){
        perror("Error open file!");
        exit(-1);
    }

    printf("Start reading \n\n");

    ans = read(data_fd, buf, BUFF_SIZE);

    int ans_write;

    while(ans > 0){
        if(ans == -1){
            perror("Error reading file from server");
            exit(-1);
        }

        ans_write = write(new_file_fd, buf, ans);
        if( ans_write < 0){
            perror("Error writing to the destination file");
            exit(-1);
        }
    }

    printf("File read complete!\n");
    printf("Close program ...\n\n");

    return 0;
}