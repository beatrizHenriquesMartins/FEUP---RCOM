#include "parser.h"

int info_parser(data_url *url, char * str){
    //a vector
    //strlen(aux) = size var STRING
    char * aux = malloc(strlen(str));
    //copy: strcpy(dst, src)
    strcpy(aux, str);

    //allocate memory
    //calloc( count,  size type);
    url->type = calloc(strlen(aux), sizeof(char));
    url->user = calloc(strlen(aux), sizeof(char));
    url->password = calloc(strlen(aux), sizeof(char));
    url->host = calloc(strlen(aux), sizeof(char));
    url->url_path = calloc(strlen(aux), sizeof(char));


    strcpy(url->type, aux);
    //strtok(str, sep) "=" getline
    aux = strtok(aux+6, ":");
    
    strcpy(url->user, aux);
    aux = strtok(NULL, "@");
    
    strcpy(url->password, aux);
    aux = strtok(NULL, "/");
    
    strcpy(url->host, aux);
    aux = strtok(NULL, "");
    
    strcpy(url->url_path, aux);

    if(strlen(aux) == 0){
        printf("%s\n",url->type);
        printf("%s\n",url->user );
        printf("%s\n",url->password);
        printf("%s\n",url->host);
        printf("%s\n",url->url_path);

        return 0;
    }
    
    return -1;
}

int parser_filename(char * filename, char path[128]){
    char * aux = malloc(strlen(path));
    // memcpy(dest, src, size_var)
    memcpy(aux, path, strlen(path));

    aux = strtok(path, "/");
    
    while(aux != NULL){
        // memset(b, int_c, length)
        // Write len bytes of value c (converted to an unsigned char) to the string b
        // returns its first argument
        memset(filename,0,strlen(filename));
        memcpy(filename, aux, strlen(aux));
        aux = strtok(NULL, "/");
    }
    
    //return 1;
    return 0;
}