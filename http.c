#include "http.h"
#include "flags.h"
#include "url.h"
#include "hb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char request[REQUEST_SIZE];

char * http_method_string(const int method) {
    switch (method) {
        case HTTP_METHOD_GET:      return HTTP_MSTR_GET;
        case HTTP_METHOD_POST:     return HTTP_MSTR_POST;
        case HTTP_METHOD_PUT:      return HTTP_MSTR_PUT;
        case HTTP_METHOD_DELETE:   return HTTP_MSTR_DELETE;
        case HTTP_METHOD_HEAD:     return HTTP_MSTR_HEAD;
        case HTTP_METHOD_OPTIONS:  return HTTP_MSTR_OPTIONS;
        case HTTP_METHOD_PROPFIND: return HTTP_MSTR_PROPFIND;
        case HTTP_METHOD_COPY:     return HTTP_MSTR_COPY;
        case HTTP_METHOD_MOVE:     return HTTP_MSTR_MOVE;
        case HTTP_METHOD_PATCH:    return HTTP_MSTR_PATCH;
        case HTTP_METHOD_TRACE:    return HTTP_MSTR_TRACE;
    }
}

char * http_version_string(const int version) {
    switch (version) {
        case HTTP_VERSION_09: return HTTP_VSTR_09;
        case HTTP_VERSION_10: return HTTP_VSTR_10;
        case HTTP_VERSION_11: return HTTP_VSTR_11;
        /*case HTTP_VERSION_20: return HTTP_VSTR_20;*/
    }
    return HTTP_VSTR_11;
}

char* http_request_string(Flags* args, UrlInfo* url) {
    char* line = (char*) malloc(LINE_SIZE);
    FILE *fd = NULL;
    const char* colon;
    int fill_user_agent = 1;
    int line_len = 0;
   
    bzero(request, REQUEST_SIZE);
    if (0 != strcasecmp("", args->file)) {
        fd = fopen(args->file, "r");
    }
    
    /* request line */
    strcpy(request, args->method);
    strcat(request, " ");
    if (strlen(url->path) > 0) {
        strcat(request, url->path);
    } else {
        strcat(request, "/");
    }
    if (strlen(url->query) > 0) {
        strcat(request, url->query);
    }
    strcat(request, " ");
    strcat(request, args->protocol);
    strcat(request, "\r\n");
    /* == end of request line */
    
    if (0 != strcmp(HTTP_VSTR_09, args->protocol)) {
        /* host line */
        strcat(request, "Host: ");
        strcat(request, url->host);
        if (url->port > 0 && url->port != 80) {
            char port[6];
            bzero(port, 6);
            sprintf(port, "%d", url->port);
            strcat(request, ":");
            strcat(request, port);
        }
        strcat(request, "\r\n");
    }
    
    if (NULL != fd) {
        while ((line = fgets((char*)line, LINE_SIZE, fd)) != NULL) {
            if (*line == '\n' || *line == '\r') {
                break;
            }
            if ((colon = strchr(line, ':')) != NULL) {
                if (0 == strncasecmp("User-Agent", line, colon - line)) {
                    fill_user_agent = 0;
                }
                line_len = strlen(line);
                if (*(line + line_len - 2) == '\r' && *(line + line_len - 1) == '\n') {
                    strcat(request, line);
                } else if (*(line + line_len - 1) == '\r' || *(line + line_len - 1) == '\n') {
                    *(line + line_len - 1) = '\0';
                    strcat(request, line);
                    strcat(request, "\r\n");
                }
            }
        }
    }
    
    if (1 == fill_user_agent) {
        strcat(request, "User-Agent: httpbench "VERSION"\r\n");
    }
    if (0 == strcmp(HTTP_VSTR_11, args->protocol)) {
        strcat(request, "Connection: close\r\n");
    }
    if (0 != strcmp(HTTP_VSTR_09, args->protocol)) {
        strcat(request, "\r\n");
    }
    
    if ((NULL != fd) && (0 != strcmp(HTTP_VSTR_09, args->protocol))) {
        while ((line_len = fread(line, 1, LINE_SIZE, fd)) > 0) {
            if (line_len < LINE_SIZE) {
                *(line + line_len) = '\0';
            }
            strcat(request, line);
        }
    }
    
    if (NULL != fd) {
        fclose(fd);
    }
    
    return request;
}

int http_scode2i(const char* code) {
    return http_code2i(atoi(code));
}

int http_code2i(const int code) {
    if (code >= 100 && code <= 102) {
        return HTTP_CODE_INDEX_1XX + code - 100; /* start from 0 */
    }
    if (code >= 200 && code <= 208) {
        return HTTP_CODE_INDEX_2XX + code - 200; /* start from 3 */
    }
    if (code >= 300 && code <= 308) {
        return HTTP_CODE_INDEX_3XX + code - 300; /* start from 13 */
    }
    if (code >= 400 && code <= 420) {
        return HTTP_CODE_INDEX_4XX + code - 400; /* start from 23 */
    }
    if (code >= 500 && code <= 511) {
        return HTTP_CODE_INDEX_5XX + code - 500;
    }
    if (code == 226) {
        return HTTP_CODE_INDEX_226; /* start from 12 */
    }
    if (code >= 422 && code <= 424) {
        return HTTP_CODE_INDEX_422 + code - 422;
    }
    if (code == 426) {
        return HTTP_CODE_INDEX_426;
    }
    if (code >= 428 && code <= 429) {
        return HTTP_CODE_INDEX_428 + code - 428;
    }
    if (code == 431) {
        return HTTP_CODE_INDEX_431;
    }
    if (code == 440) {
        return HTTP_CODE_INDEX_440;
    }
    if (code == 444) {
        return HTTP_CODE_INDEX_444;
    }
    if (code >= 449 && code <= 451) {
        return HTTP_CODE_INDEX_449 + code - 449;
    }
    if (code >= 494 && code <= 499) {
        return HTTP_CODE_INDEX_494 + code - 494;
    }
    if (code >= 520 && code <= 524) {
        return HTTP_CODE_INDEX_520 + code - 520;
    }
    if (code >= 598 && code <= 599) {
        return HTTP_CODE_INDEX_598 + code - 598;
    }
    return -1;
}

int http_i2code(const int i) {
    if (i >= HTTP_CODE_INDEX_1XX && i < HTTP_CODE_INDEX_2XX) {
        return i + 100 - HTTP_CODE_INDEX_1XX;
    }
    if (i >= HTTP_CODE_INDEX_2XX && i < HTTP_CODE_INDEX_226) {
        return i + 200 - HTTP_CODE_INDEX_2XX;
    }
    if (i >= HTTP_CODE_INDEX_3XX && i < HTTP_CODE_INDEX_4XX) {
        return i + 300 - HTTP_CODE_INDEX_3XX;
    }
    if (i >= HTTP_CODE_INDEX_4XX && i < HTTP_CODE_INDEX_422) {
        return i + 400 - HTTP_CODE_INDEX_4XX;
    }
    if (i >= HTTP_CODE_INDEX_5XX && i < HTTP_CODE_INDEX_520) {
        return i + 500 - HTTP_CODE_INDEX_5XX;
    }
    if (HTTP_CODE_INDEX_226 == i) {
        return 226;
    }
    if (i >= HTTP_CODE_INDEX_422 && i < HTTP_CODE_INDEX_426) {
        return i + 422 - HTTP_CODE_INDEX_422;
    }
    if (HTTP_CODE_INDEX_426 == i) {
        return 426;
    }
    if (i >= HTTP_CODE_INDEX_428 && i < HTTP_CODE_INDEX_431) {
        return i + 428 - HTTP_CODE_INDEX_428;
    }
    if (HTTP_CODE_INDEX_431 == i) {
        return 431;
    }
    if (HTTP_CODE_INDEX_440 == i) {
        return 440;
    }
    if (HTTP_CODE_INDEX_444 == i) {
        return 444;
    }
    if (i >= HTTP_CODE_INDEX_449 && i < HTTP_CODE_INDEX_494) {
        return i + 449 - HTTP_CODE_INDEX_449;
    }
    if (i >= HTTP_CODE_INDEX_494 && i < HTTP_CODE_INDEX_5XX) {
        return i + 494 - HTTP_CODE_INDEX_494;
    }
    if (i >= HTTP_CODE_INDEX_520 && i < HTTP_CODE_INDEX_598) {
        return i + 520 - HTTP_CODE_INDEX_520;
    }
    if (i >= HTTP_CODE_INDEX_598 && i < HTTP_CODE_COUNT_ALL) {
        return i + 598 - HTTP_CODE_INDEX_598;
    }
    return -1;
}

char* http_code_array_encode(char* buf, const int* arr) {
    buf = (char *) malloc(CODES_ENCODE_SIZE);
    bzero(buf, CODES_ENCODE_SIZE);
    int i, printed, pos = 0;
    for (i = 0; i < HTTP_CODE_COUNT_ALL; i++) {
        printed = sprintf(buf + pos, "%d=%d,", i, arr[i]);
        pos += printed;
    }
    return buf;
}

int*  http_code_array_decode(int* buf, const char* str) {
    const char* p, *ip = str, *vp;
    char ibuf[12], vbuf[12];
    for (p = str; *p != '\0' && p < (str + CODES_ENCODE_SIZE); p++) {
        if (*p == '=') {
            memcpy(ibuf, ip, p - ip);
            ibuf[p - ip] = '\0';
            vp = p + 1;
        } else if (*p == ',') {
            memcpy(vbuf, vp, p - vp);
            vbuf[p - vp] = '\0';
            *(buf + atoi(ibuf)) = atoi(vbuf);
            ip = p + 1;
        }
    }
    return buf;
}