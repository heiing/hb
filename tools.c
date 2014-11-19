#include "tools.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char* str_trim(const char *str) {
    return str_trim_ex(str, strlen(str), NULL, 0, 3);
}
char* str_ltrim(const char *str) {
    return str_trim_ex(str, strlen(str), NULL, 0, 1);
}
char* str_rtrim(const char *str) {
    return str_trim_ex(str, strlen(str), NULL, 0, 2);
}
char* str_trim_chars(const char *str, const char *chars) {
    return str_trim_ex(str, strlen(str), chars, strlen(chars), 3);
}
char* str_ltrim_chars(const char *str, const char *chars) {
    return str_trim_ex(str, strlen(str), chars, strlen(chars), 1);
}
char* str_rtrim_chars(const char *str, const char *chars) {
    return str_trim_ex(str, strlen(str), chars, strlen(chars), 2);
}
char* str_trim_ex(const char *str, int len, const char *what, int what_len, int mode) {
    register int i;
    int trimmed = 0;
    char mask[256];
    
    if (what) {
        char_mask((unsigned char*)what, what_len, mask);
    } else {
        char_mask((unsigned char*)"\t\n\v\r \0", 6, mask);
    }
    
    if (1 & mode) {
        for (i = 0; i < len; i++) {
            if (mask[(unsigned char)str[i]]) {
                trimmed++;
            } else {
                break;
            }
        }
        len -= trimmed;
        str += trimmed;
    }
    if (2 & mode) {
        for (i = len - 1; i >= 0; i--) {
            if (mask[(unsigned char)str[i]]) {
                len--;
            } else {
                break;
            }
        }
    }
    
    char *ret;
    ret = (char *) malloc(len + 1);
    memcpy(ret, str, len);
    *(ret + len) = '\0';
    return ret;
}

void char_mask (const char *input, int len, char *mask) {
    const char * end;
    char c;
    memset(mask, 0, 256);
    for (end = input + len; input < end; input++) {
        c = *input;
        if ((input + 3 < end) && (input[1] == '.') && (input[2] == '.') && (input[3] >= c)) {
            memset(mask + c, 1, input[3] - c + 1);
            input += 3;
        } else {
            mask[c] = 1;
        }
    }
}
