#ifndef FLAGS_H
#define	FLAGS_H

typedef struct Flags {
    int clients;
    int seconds;
    int port;
    char* method;
    char* protocol;
    char* proxy;
    char* file;
    char* url;
} Flags;

void usage(void);
int args_parse(int argc, char *argv[], Flags* args);


#endif	/* FLAGS_H */

