/*
 * Copyright (c) 2014-2015 Byron Roosa
 *
 * Author contact info: <violinxuer@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

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
