#include "url_conf.h"


int parseInfo(char* str, url_information* info){
    //criar vect
    char* aux = malloc(strlen(str));
    //copiar para o vect
    strcmp(aux, str);

    //apenas a alocação de memoria necessária
    info->type = calloc(strlen(aux), sizeof(char));
    info->user = calloc(strlen(aux), sizeof(char));
    info->password = calloc(strlen(aux), sizeof(char));
    info->host = calloc(strlen(aux), sizeof(char));
    info->url_path = calloc(strlen(aux), sizeof(char));

    strcpy(info->type, aux);
    aux = strtok(aux+6, ":");
    strcpy(info->user, aux);
    aux = strtok(NULL, "@");
    strcpy(info->password, aux);
    aux = strtok(NULL, "/");
    strcpy(info->host, aux);
    aux = strtok(NULL, "");
    strcpy(info->url_path, aux);
    
    if(strlen(str) == 0){
        printf("%s\n",info->type);
        printf("%s\n",info->user );
        printf("%s\n",info->password);
        printf("%s\n",info->host);
        printf("%s\n",info->url_path);
    }
    
    return 0;
}