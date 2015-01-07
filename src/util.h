#ifndef _UTIL_H_INCLUDED
#define _UTIL_H_INCLUDED

#define SUCCESS 0
#define ERR_IO_READ_FAIL 1
#define ERR_IO_READ_OVERFLOW 2

#define DEBUG_ENABLED

#ifdef DEBUG_ENABLED

//Define our own debug printf wrapper. Note that this depends on the C99 standard (__VA_ARGS__)
#define DEBUG_OUTPUT(...) printf(__VA_ARGS__)

#else

#define DEBUG_OUTPUT(...)

#endif

void debug_output(char *str);

//char *stdin_read_line_dynamic(void);
int stdin_read_line(char *str, unsigned int max_bytes);

//char *stdin_prompt_line_dynamic(const char *prompt);
int stdin_prompt_line(const char *prompt, char *str, unsigned int max_bytes);

#endif
