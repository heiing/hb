#ifndef HTTP_HEAD_H
#define	HTTP_HEAD_H

#include "flags.h"
#include "url.h"

#define HTTP_METHOD_GET      1
#define HTTP_METHOD_POST     2
#define HTTP_METHOD_PUT      3
#define HTTP_METHOD_DELETE   4
#define HTTP_METHOD_HEAD     5
#define HTTP_METHOD_OPTIONS  6
#define HTTP_METHOD_PROPFIND 7
#define HTTP_METHOD_COPY     8
#define HTTP_METHOD_MOVE     9
#define HTTP_METHOD_PATCH    10
#define HTTP_METHOD_TRACE    11

#define HTTP_MSTR_GET      "GET"
#define HTTP_MSTR_POST     "POST"
#define HTTP_MSTR_PUT      "PUT"
#define HTTP_MSTR_DELETE   "DELETE"
#define HTTP_MSTR_HEAD     "HEAD"
#define HTTP_MSTR_OPTIONS  "OPTIONS"
#define HTTP_MSTR_PROPFIND "PROPFIND"
#define HTTP_MSTR_COPY     "COPY"
#define HTTP_MSTR_MOVE     "MOVE"
#define HTTP_MSTR_PATCH    "PATCH"
#define HTTP_MSTR_TRACE    "TRACE"

#define HTTP_VERSION_09 0
#define HTTP_VERSION_10 1
#define HTTP_VERSION_11 2
/*#define HTTP_VERSION_20 3*/

#define HTTP_VSTR_09 ""
#define HTTP_VSTR_10 "HTTP/1.0"
#define HTTP_VSTR_11 "HTTP/1.1"
/*#define HTTP_VSTR_20 "HTTP/2.0"*/

/* count of http code */
#define HTTP_CODE_COUNT_1XX  3
#define HTTP_CODE_COUNT_2XX 10
#define HTTP_CODE_COUNT_3XX  9
#define HTTP_CODE_COUNT_4XX 41
#define HTTP_CODE_COUNT_5XX 19
#define HTTP_CODE_COUNT_ALL 82

#define HTTP_CODE_INDEX_1XX  0
#define HTTP_CODE_INDEX_2XX  3
#define HTTP_CODE_INDEX_3XX 13
#define HTTP_CODE_INDEX_4XX 22
#define HTTP_CODE_INDEX_5XX 63
#define HTTP_CODE_INDEX_226 12
#define HTTP_CODE_INDEX_422 43
#define HTTP_CODE_INDEX_426 46
#define HTTP_CODE_INDEX_428 48
#define HTTP_CODE_INDEX_431 50
#define HTTP_CODE_INDEX_440 51
#define HTTP_CODE_INDEX_444 52
#define HTTP_CODE_INDEX_449 53
#define HTTP_CODE_INDEX_494 56
#define HTTP_CODE_INDEX_520 75
#define HTTP_CODE_INDEX_598 80


/* 4K */
#define LINE_SIZE 4096

/* 2M */
#define REQUEST_SIZE (1024 * 1024 * 2)

/* 2M */
#define REPONSE_SIZE (1024 * 1024 * 2)

#define CODES_ENCODE_SIZE (12 * HTTP_CODE_COUNT_ALL)

char* http_method_string(const int method);
char* http_version_string(const int version);
char* http_request_string(Flags* args, UrlInfo* url);

int http_code2i(const int code);
int http_scode2i(const char* code);
int http_i2code(const int i);

char* http_code_array_encode(char* buf, const int* arr);
int*  http_code_array_decode(int* buf, const char* str);

#endif	/* HTTP_HEAD_H */