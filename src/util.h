#ifndef _UTIL_H_INCLUDED
#define _UTIL_H_INCLUDED

#define SUCCESS 0
#define ERR_IO_READ_FAIL 1
#define ERR_IO_READ_OVERFLOW 2

char *stdin_read_line_dynamic(void);
int stdin_read_line(char *str, unsigned int max_bytes);

char *stdin_prompt_line_dynamic(const char *prompt);
int stdin_prompt_line(const char *prompt, char *str, unsigned int max_bytes);

#endif
