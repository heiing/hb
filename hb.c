#include "hb.h"
#include "url.h"
#include "flags.h"
#include "http.h"
#include "socket.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>


int speed = 0, failed = 0, bytes = 0;
int status[HTTP_CODE_COUNT_ALL];

int thepipe[2];

volatile int expired = 0;
static void alarm_handler(int signal) {
    expired = 1;
}

void work(const char* host, unsigned short port, const char* request, const Flags* args) {
    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sa.sa_flags = 0;
    if (sigaction(SIGALRM, &sa, NULL)) {
        exit(3);
    }
    alarm(args->seconds);
    
    int request_length = strlen(request);
    int sock, read_bytes, first_line, status_code;
    char buf[SOCK_READ_BUF_SIZE];
    char* status_pos;
    
    nexttry:
    while (1) {
        if (expired) {
            if (failed > 0) {
                failed--;
            }
            return;
        }
        
        sock = Socket(host, port);
        if (sock < 0) {
            failed++;
            continue;
        }
        
        if (request_length != write(sock, request, request_length)) {
            failed++;
            close(sock);
            continue;
        }
        
        first_line = 1;
        while (1) {
            if (expired) {
                break;
            }
            read_bytes = read(sock, buf, SOCK_READ_BUF_SIZE);
            if (read_bytes < 0) {
                failed++;
                close(sock);
                goto nexttry;
            } else {
                if ((1 == first_line) && (0 == strncasecmp("HTTP", buf, 4))) {
                    status_pos = strchr(buf, ' ');
                    if ((NULL != status_pos) && (status_pos + 4 < buf + SOCK_READ_BUF_SIZE)) {
                        status_pos++;
                        *(status_pos + 3) = '\0';
                        status_code = http_scode2i(status_pos);
                        if (status_code >= 0) {
                            status[status_code]++;
                        }
                    }
                }
                first_line = 0;
                if (read_bytes == 0) {
                    break;
                } else {
                    bytes += read_bytes;
                }
            }
        }
        
        if (close(sock)) {
            failed++;
            continue;
        }
        
        speed++;
    }
}

int main(int argc, char *argv[]) {
    
    /* 参数解析 */
    Flags* args = malloc(sizeof(Flags));
    
    /* 参数验证 */
    switch (args_parse(argc, argv, args)) {
        case 1:
            usage();
            return 1;
        case 2:
            printf("httpbench "VERSION"\n");
            return 0;
        case -1:
            fprintf(stderr, "missing proxy host\n");
            return 2;
        case -2:
            fprintf(stderr, "missing proxy port\n");
            return 2;
        case -3:
            fprintf(stderr, "missing url\n");
            return 2;
    }

    /* 解析参数中的 URL */
    UrlInfo *url = url_parse(args->url);
    /* 检查协议 */
    if (0 != strcasecmp("http", url->scheme)) {
        fprintf(stderr, "only HTTP is supported\n");
        return 2;
    }
    if (strlen(url->host) <= 0) {
        fprintf(stderr, "missing host\n");
        return 2;
    }
    if ((0 != strcasecmp("", args->file)) && (0 != access(args->file, R_OK))) {
        fprintf(stderr, "connot read file: %s\n", args->file);
        return 2;
    }
    if (args->port <= 0 || args->port > 65535) {
        args->port = url->port > 0 && url->port <= 65535 ? url->port : 80;
    }
    printf("Running httpbench %s Copyright (c) heiing 2014, GPL Open Source Software.\n", VERSION);
    printf("%s %s using %s\n%d client, running %d seconds",
        args->method, args->url, args->protocol, args->clients, args->seconds);
    if (0 != strcmp("", args->proxy)) {
        printf(" via proxy %s:%d", args->proxy, args->port);
    }
    printf(".\n");

    char* request = http_request_string(args, url);
    printf("\nRequest:\n================\n%s================\n", request);
    
    char* host = ((0 == strcmp("", args->proxy)) ? url->host : args->proxy);
    int port = args->port;
    
    int test_socket = Socket(host, port);
    if (test_socket < 0) {
        fprintf(stderr, "\nConnect to server faild.\n");
        return 1;
    }
    close(test_socket);
    
    if (pipe(thepipe)) {
        perror("pipe faild.");
        return 3;
    }
    
    int i;
    pid_t pid;
    FILE* f;
    
    for (i = 0; i < args->clients; i++) {
        pid = fork();
        if (pid <= (pid_t)0) {
            sleep(1);
            break;
        }
    }
    
    if (pid < (pid_t)0) {
        fprintf(stderr, "worker no. %d ", i);
        perror("fork failed.");
        return 3;
    }
    
    if (pid == (pid_t)0) {
        work(host, port, request, args);
        f = fdopen(thepipe[1], "w");
        if (NULL == f) {
            perror("open pipe for writing failded.");
            return 3;
        }
        char* s;
        fprintf(f, "%d %d %d %s\n", speed, failed, bytes, http_code_array_encode(s, status));
        fclose(f);
        return 0;
    } else {
        f = fdopen(thepipe[0], "r");
        if (NULL == f) {
            perror("open pipe for reading failded.");
            return 3;
        }
        setvbuf(f, NULL, _IONBF, 0);
        
        int x, y, z, i, clients = args->clients;
        speed  = 0;
        failed = 0;
        bytes  = 0;
        char* s = (char*) malloc(CODES_ENCODE_SIZE);
        int* c = (int*) malloc(sizeof(int) * HTTP_CODE_COUNT_ALL);
        
        while (1) {
            bzero(s, CODES_ENCODE_SIZE);
            bzero(c, sizeof(int) * HTTP_CODE_COUNT_ALL);
            pid = fscanf(f, "%d %d %d %s", &x, &y, &z, s);
            if (pid < 2) {
                fprintf(stderr, "Some of our childrens died.\n");
                break;
            }
            speed  += x;
            failed += y;
            bytes  += z;
            c = http_code_array_decode(c, s);
            for (i = 0; i < HTTP_CODE_COUNT_ALL; i++) {
                if (c[i] > 0) {
                    status[i] += c[i];
                }
            }
            
            if (--clients == 0) {
                break;
            }
        }
        
        fclose(f);
        
        printf(
            "\nSpeed = %d pages/minute, %d bytes/second,\n"
            "Request: %d success, %d failed.\n"
            , (int)((speed + failed) / ((args->seconds) / 60.0f))
            , (int)(bytes / (float)(args->seconds))
            , speed
            , failed
            );
        printf("\nHttp Status Report:\n");
        for (i = 0; i < HTTP_CODE_COUNT_ALL; i++) {
            if (status[i] > 0) {
                printf("%d = %d\n", http_i2code(i), status[i]);
            }
        }
    }
    
    return 0;
}
