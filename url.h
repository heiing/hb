#ifndef URL_H
#define	URL_H

#define LENGTH (end - begin)

typedef struct UrlInfo {
    char *scheme;
    char *host;
    unsigned short port;
    char *user;
    char *pass;
    char *path;
    char *query;
    char *fragment;
} UrlInfo;

UrlInfo *url_parse(const char *url);

void url_free(UrlInfo *url);

unsigned short protocal_port(const char* scheme);

#endif	/* URL_H */

