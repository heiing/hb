#include "url.h"
#include "tools.h"
#include <string.h>
#include <stdlib.h>

void url_free(UrlInfo* url) {
    if (url->fragment)
        free(url->fragment);
    if (url->host)
        free(url->host);
    if (url->pass)
        free(url->pass);
    if (url->path)
        free(url->path);
    if (url->query)
        free(url->query);
    if (url->scheme)
        free(url->scheme);
    if (url->user)
        free(url->user);
    free(url);
}

UrlInfo* url_parse (const char* url) {
    /* url = "http://username:password@host:80/path/file?arg=value&arg2=val2#anchor" */
    UrlInfo* ui = (UrlInfo *) malloc(sizeof(UrlInfo));
    char const *p, *begin, *end;
    /* trim space */
    for (begin = url, end = url + strlen(url); ((unsigned char)(*begin) <= 32) && (begin < end); begin++);
    for (; ((unsigned char)(*(end - 1)) <= 32) && (end > begin); end--);
    
    /* find the pointer for some special chars */
    const char* colon = strchr(begin, ':'), *slash = strchr(begin, '/'),
        *at = strchr(begin, '@'), *question = strchr(begin, '?'), *sharp = strchr(begin, '#');
    
    /* parse scheme */
    /* url = "http://username:password@host:80/path/file?arg=value&arg2=val2#anchor" */
    if ((colon > begin) && (colon + 1 == slash) && (*(slash + 1) == '/') && (slash + 1 < end)) {
        ui->scheme = (char *) malloc(colon - begin + 1);
        memcpy(ui->scheme, begin, colon - begin);
        ui->scheme[colon - begin] = '\0';
        begin = colon + 3;
        colon = strchr(begin, ':');
        slash = strchr(begin, '/');
    } else {
        ui->scheme = (char *) malloc(1);
        ui->scheme[0] = '\0';
    }
    
    /* parse fragment */
    /* url = "username:password@host:80/path/file?arg=value&arg2=val2#anchor" */
    if (sharp >= begin && sharp < end) {
        ui->fragment = (char *) malloc(end - sharp);
        memcpy(ui->fragment, sharp + 1, end - sharp - 1);
        ui->fragment[end - sharp - 1] = '\0';
        end = sharp;
    } else {
        ui->fragment = (char *) malloc(1);
        ui->fragment[0] = '\0';
    }
    
    /* parse query */
    /* url = "username:password@host:80/path/file?arg=value&arg2=val2" */
    if (question >= begin && question < end) {
        ui->query = (char *) malloc(end - question);
        memcpy(ui->query, question + 1, end - question - 1);
        ui->query[end - question - 1] = '\0';
        end = question;
    } else {
        ui->query = (char *) malloc(1);
        ui->query[0] = '\0';
    }
    
    /* parse path */
    /* url = "username:password@host:80/path/file" */
    if (slash >= begin && slash < end) {
        ui->path = (char *) malloc(end - slash + 1);
        memcpy(ui->path, slash, end - slash);
        ui->path[end - slash] = '\0';
        end = slash;
    } else {
        ui->path = (char *) malloc(1);
        ui->path[0] = '\0';
    }
    
    /* parse user and pass */
    /* url = "username:password@host:80" */
    if (at > begin && at < end && begin < end) {
        if (colon > begin && colon < at) {
            ui->user = (char *) malloc(colon - begin + 1);
            ui->pass = (char *) malloc(at - colon);
            memcpy(ui->user, begin, colon - begin);
            memcpy(ui->pass, colon + 1, at - colon - 1);
            ui->user[colon - begin] = '\0';
            ui->pass[at - colon - 1] = '\0';
        } else {
            ui->user = (char *) malloc(at - begin + 1);
            memcpy(ui->user, begin, at - begin);
            ui->user[at - begin] = '\0';
            ui->pass = (char *) malloc(1);
            ui->pass[0] = '\0';
        }
        begin = at + 1;
    } else {
        ui->user = (char *) malloc(1);
        ui->pass = (char *) malloc(1);
        ui->user[0] = '\0';
        ui->pass[0] = '\0';
    }
/*printf("\nURL: %s\nUSER: %s\nPASS: %s\nQuery: %s\n", url, ui->user, ui->pass, ui->query);*/
    
    /* parse host and port */
    /* url = "host:80" */
    if (begin < end) {
        colon = strchr(begin, ':');
        if (colon > begin && colon < end) {
            ui->host = (char *) malloc(colon - begin + 1);
            memcpy(ui->host, begin, colon - begin);
            ui->host[colon - begin] = '\0';
            if (end - colon <= 6) {
                char port_chars[5] = { 0, 0, 0, 0, 0};
                memcpy(port_chars, colon + 1, end - colon - 1);
                int port = atoi(port_chars);
                ui->port = (port > 0 && port < 65536) ? port : 0;
            } else {
                ui->port = 0;
            }
        } else {
            ui->host = (char *) malloc(end - begin + 1);
            memcpy(ui->host, begin, end - begin);
            ui->host[end - begin] = '\0';
            ui->port = 0;
        }
    } else {
        ui->host = (char *) malloc(1);
        ui->host[0] = '\0';
        ui->port = 0;
    }
    
    return ui;
}

unsigned short protocal_port(const char* scheme) {
    if (strcasecmp(scheme, "http") == 0) {
        return 80;
    }
    if (strcasecmp(scheme, "https") == 0) {
        return 443;
    }
    if (strcasecmp(scheme, "ftp") == 0) {
        return 21;
    }
    if (strcasecmp(scheme, "ssh") == 0) {
        return 22;
    }
    return 0;
}
