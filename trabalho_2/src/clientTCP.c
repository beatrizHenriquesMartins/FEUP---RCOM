#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <strings.h>

#include "tcpProtocol.h"

#define SERVER_PORT 6000
#define SERVER_ADDR "192.168.28.96"

// login anonymous
// password anything


int init_TCP_protocol(char *address, int port) {
	int	sockfd;
    struct sockaddr_in server_addr;

    /*server address handling*/
	bzero((char*)&server_addr,sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	/*32 bit Internet address network byte ordered*/
	server_addr.sin_addr.s_addr = inet_addr(address);
	/*server TCP port must be network byte ordered */
	server_addr.sin_port = htons(port);

    /*open an TCP socket*/
	if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {
		perror("socket()");
      	exit(-1);
    }

	/*connect to the server*/
    if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0){
		perror("connect()");
		exit(-1);
	}

    return sockfd;
}

int main(int argc, char** argv){
	if (argc != 2) {
    	perror("\n\n *Invalid arguments* try: ./clientDownload ftp://[<user>:<password>@]<host>/<url-path>\n\n");
		return -1;
  	}

	data_url *url_info = malloc(sizeof(data_url));
	info_parser(url_info, argv[1]);

	int aux_user = strcmp(url_info->user,"anonymous");

	if(aux_user != 0){
		perror("* User Error * ");
		exit(-1);
	}

  	struct addrinfo hints;
	struct addrinfo  *res;

  	memset(&hints, 0, sizeof(hints));
  	hints.ai_family = AF_INET;
  	hints.ai_socktype = SOCK_STREAM;

    int aux_address_info = getaddrinfo(url_info->host, "21", &hints, &res);
  	if (aux_address_info != 0) {
    	perror("* Host Error *\n");
    	exit(0);
  	}

  	//open an TCP socket
	int socket_fd;
  	if ((socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0) {
    	perror("socket()");
    	exit(0);
  	}
	
	char answer[MAX_SIZE];

  	connection(socket_fd, res);
  	read_socket(socket_fd, answer);


  	login(socket_fd, url_info);

	printf("%s\n\n", "Logged in");

	char answer_ip[MAX_SIZE];
	set_passive_mode(socket_fd, answer_ip);
	printf("Passive mode active! \n");

	char ip_adress[128];
	char port1[128];
	char port2[128];

	get_ip(answer_ip, ip_adress, port1, port2);
	printf("Ip : %s\n", ip_adress);

	int port = atoi(port1) * 256 + atoi(port2);
	printf("Port : %d\n\n", port);

	int data_fd = init_TCP_protocol(ip_adress, port);

	asking_file(socket_fd,url_info);

	char *filename = calloc(MAX_SIZE, sizeof(char));
	parser_filename(url_info->url_path, filename);
	printf("filename: %s\n", filename);

	read_file(data_fd,filename);

	close(data_fd);
  	close(socket_fd);
  	exit(0);
}


