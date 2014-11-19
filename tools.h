#ifndef TOOLS_H
#define	TOOLS_H

#define is_alpha(x) ((((x) >= 65 && (x) <= 90) || ((x) >= 97 && (x) <= 122)) ? 1 : 0)
#define is_digit(x) (((x) >= 48 && (x) <= 57) ? 1 : 0)

char* str_trim(const char *str);
char* str_ltrim(const char *str);
char* str_rtrim(const char *str);
char* str_trim_chars(const char *str, const char *chars);
char* str_ltrim_chars(const char *str, const char *chars);
char* str_rtrim_chars(const char *str, const char *chars);
char* str_trim_ex(const char *str, int len, const char *what, int what_len, int mode);

void char_mask (const char *input, int len, char *mask);
void str_copy(char* dst, const char* src);

#endif	/* TOOLS_H */

