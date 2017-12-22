#ifndef __PARSE_URL_H__
#define  __PARSE_URL_H__

#define LINK_HEADER "ftp://"
#define DEFAULT_USER "anonymous"
#define DEFAULT_PASSWORD "mail@domain"

// if there is no host substitute 
// by anonymous, mail@domain

typedef struct {
  char user[256];
  char password[256];
  char urlHost[256];
  struct hostent* infoHost;
  char path[256];
  char filename[256];
} urlInfo ;

/**
  * @brief Retrieves the username, password and download file name from a ftp URL
  * @param url URL with the information to parseURL
  * @param info Struct to complete with the information read from the URL
  * @returns 0 if URL is correct
  */
int parseURL(char url[], urlInfo* info);

#endif
