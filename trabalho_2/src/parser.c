#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "parser.h"

/**
 * Init Default User (Anonymous:mail@domain)
 * @method initDefaultUser
 * @param  info            link info
 */
void initDefaultUser(urlInfo *info) {
  memcpy(info->user, DEFAULT_USER, strlen(DEFAULT_USER) + 1);
  memcpy(info->password, DEFAULT_PASSWORD, strlen(DEFAULT_PASSWORD) + 1);
};

/**
 * Init User
 * @method initUser
 * @param  info     link info
 * @param  url      link
 * @param  atPos    slash pos
 * @return
 */
int initUser(urlInfo *info, char *url, char *atPos) {
  char *slashChar = strchr(url, '/'); // slashChar can't be null
  slashChar += 2;
  char *password = strchr(slashChar, ':');
  if (password == NULL) {
    fprintf(stderr, "There must be a ':' separating the username and "
                    "password!' on the link\n");
    return 1;
  }
  memcpy(info->user, slashChar, password - slashChar);
  info->user[password - slashChar] = 0;
  password++;
  memcpy(info->password, password, atPos - password);
  info->password[atPos - password] = 0;
  return 0;
}

/**
 * parse link to urlInfo struct
 * @method parseURL
 * @param  url      string of link
 * @param  info     urlInfo
 * @return
 */
int parseURL(char url[], urlInfo *info) {
  if (strncmp(url, LINK_HEADER, strlen(LINK_HEADER)) != 0) {
    fprintf(stderr, "Your link must begin with 'ftp://'\n");
    return 1;
  }
  char *atPos = strrchr(url, '@');
  if (atPos == NULL) {
    initDefaultUser(info);
    atPos = url + strlen("ftp://");
  } else {
    if (initUser(info, url, atPos) != 0)
      return 1;
    atPos++;
  }

  // find slashes for parsing of information
  char *firstSlashChar = strchr(atPos, '/');
  memcpy(info->urlHost, atPos, firstSlashChar - atPos);
  info->urlHost[firstSlashChar - atPos] = 0;

  char *lastSlashChar = strrchr(url, '/');
  lastSlashChar++;
  memcpy(info->path, firstSlashChar, lastSlashChar - firstSlashChar);
  info->path[lastSlashChar - firstSlashChar] = 0;

  memcpy(info->filename, lastSlashChar, strlen(lastSlashChar) + 1);
  if ((info->infoHost = gethostbyname(info->urlHost)) == NULL) {
    herror(info->urlHost);
    exit(1);
  }

  return 0;
}
