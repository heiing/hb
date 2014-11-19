#include "flags.h"
#include "tools.h"
#include "http.h"
#include <stdio.h>
#include <libio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

void usage(void) {
    fprintf(stderr,
        "httpbench [option]... URL\n"
        "  -h | --help                  Print this information and exit.\n"
        "  -v | --version               Display program version and exit.\n"
        "  -c | --clients <n>           Run <n> clients, default one.\n"
        "  -t | --time <second>         Run <second> seconds, default 30.\n"
        "  -m | --method <n>            Use HTTP methodm, default GET:\n"
        "                                  GET,POST,PUT,DELETE,HEAD,TRACE,PATCH\n"
        "                                  PROPFIND,COPY,MOVE\n"
        "  -p | --protocol <n>          Use HTTP protocol, default 2:\n"
        "                                 0 - HTTP/0.9\n"
        "                                 1 - HTTP/1.0\n"
        "                                 2 - HTTP/1.1\n"
        "  -f | --file <path>           Use file for POST/PUT method.\n"
        "  -P | --proxy <server:port>   Use proxy for request.\n"
    );
}

/**
 * return:
 *   1 - print help;
 *   2 - print version;
 *  -1 - missing proxy hostname
 *  -2 - missing proxy port
 *  -3 - missing url
 * @param argc
 * @param argv
 * @param args
 * @return 
 */
int args_parse(int argc, char *argv[], Flags* args) {
    if (argc == 1) {
        return 1;
    }

    static const struct option long_options[] = {
        {"help",    no_argument,       NULL,          'h'},
        {"version", no_argument,       NULL,          'v'},
        {"clients", required_argument, NULL,          'c'},
        {"time",    required_argument, NULL,          't'},
        {"method",  required_argument, NULL,          'm'},
        {"protocal",required_argument, NULL,          'p'},
        {"file",    required_argument, NULL,          'f'},
        {"proxy",   required_argument, NULL,          'P'},
        {NULL,      0,                 NULL,           0}
    };
    
    int opt       = 0;
    int opt_index = 0;
    int opt_len   = 0;
    char* tmp     = NULL;
    
    while ((opt = getopt_long(argc, argv, "hvc:t:m:p:f:P:", long_options, &opt_index)) != EOF) {
        switch (opt) {
            case 0:
                break;
            case  'h':
                return 1;
                break;
            case 'v':
                return 2;
                break;
            case 'c':
                args->clients = atoi(optarg);
                break;
            case 't':
                args->seconds = atoi(optarg);
                break;
            case 'm':
                if (strcasecmp(optarg, HTTP_MSTR_POST) == 0) {
                    args->method = HTTP_MSTR_POST;
                } else if (strcasecmp(optarg, HTTP_MSTR_PUT) == 0) {
                    args->method = HTTP_MSTR_PUT;
                } else if (strcasecmp(optarg, HTTP_MSTR_DELETE) == 0) {
                    args->method = HTTP_MSTR_DELETE;
                } else if (strcasecmp(optarg, HTTP_MSTR_HEAD) == 0) {
                    args->method = HTTP_MSTR_HEAD;
                } else if (strcasecmp(optarg, HTTP_MSTR_OPTIONS) == 0) {
                    args->method = HTTP_MSTR_OPTIONS;
                } else if (strcasecmp(optarg, HTTP_MSTR_PROPFIND) == 0) {
                    args->method = HTTP_MSTR_PROPFIND;
                } else if (strcasecmp(optarg, HTTP_MSTR_COPY) == 0) {
                    args->method = HTTP_MSTR_COPY;
                } else if (strcasecmp(optarg, HTTP_MSTR_MOVE) == 0) {
                    args->method = HTTP_MSTR_MOVE;
                } else if (strcasecmp(optarg, HTTP_MSTR_TRACE) == 0) {
                    args->method = HTTP_MSTR_TRACE;
                } else {
                    args->method = HTTP_MSTR_GET;
                }
                break;
            case 'p':
                switch (atoi(optarg)) {
                    case 0:
                        args->protocol = HTTP_VSTR_09;
                        break;
                    case 1:
                        args->protocol = HTTP_VSTR_10;
                        break;
                    case 2:
                    default:
                        args->protocol = HTTP_VSTR_11;
                        break;
                }
                break;
            case 'f':
                opt_len = strlen(optarg);
                args->file = (char*) malloc(opt_len + 1);
                memcpy(args->file, optarg, opt_len);
                args->file[opt_len] = '\0';
                break;
            case 'P':
                opt_len = strlen(optarg);
                tmp = strrchr(optarg, ':');
                if (tmp == NULL) {
                    args->proxy = (char*) malloc(opt_len + 1);
                    memcpy(args->proxy, optarg, opt_len);
                    args->proxy[opt_len] = '\0';
                    break;
                }
                args->proxy = (char*) malloc(tmp - optarg + 1);
                memcpy(args->proxy, optarg, tmp - optarg);
                args->proxy[tmp - optarg] = '\0';
                if (tmp == optarg) {
                    return -1;
                }
                if (tmp == optarg + opt_len - 1) {
                    return -2;
                }
                *tmp = '\0';
                args->port = atoi(tmp + 1);
                break;
        }
    }
    if (argc == optind) {
        return -3;
    }
    if (args->clients <= 0) {
        args->clients = 1;
    }
    if (args->clients > 30000) {
        args->clients = 30000;
    }
    if (!args->file) {
        args->file = "";
    }
    if (!args->method) {
        args->method = HTTP_MSTR_GET;
    }
    if (!args->protocol) {
        args->protocol = HTTP_VSTR_11;
    }
    if (!args->proxy) {
        args->proxy = "";
    }
    if (args->seconds == 0) {
        args->seconds = 30;
    }
    args->url = argv[optind];
    return 0;
}