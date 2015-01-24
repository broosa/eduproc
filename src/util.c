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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

//Reads a line from stdin, uses dynamic allocation for strings (Returns char *)
/*char *stdin_read_line_dynamic(void)
{
	char *dst = NULL;
	size_t bytes_allocated = 0;
	size_t bytes_read = getline(&dst, &bytes_allocated, stdin);
	return dst;
}*/

//Splits a string on a specified delimiter (probably really slow)
//Returns an array of pointers to the tokens within the
//original string
char **split_string(char *src, const char *del, int *count) {
	char *delim;
	int token_count = 0;
	
	//If the delimiter is null, use a space as a default. 
	//Otherwise, use the specified delimeter
	if (src == NULL) {
		delim = " ";
	} else {
		delim = (char *) del;
	}
	
	//Figure out how many tokens there will be
	char src_copy[strlen(src) + 1];
	strcpy(src_copy, src);
	
	char **tokens;
	
	//If strtok returns null, then there probably isn't any place
	//to actually split the token
	if (strtok(src_copy, delim) == NULL) {
		token_count = 0;
		tokens = NULL;
		goto exit;
	} else {
		token_count = 1;
		while (strtok(NULL, delim) != NULL) {
			token_count++;
		}
	}
	//Now use strtok to actually split the string
	
	//We have one extra space for a null (to signify the end of
	//the array
	tokens = malloc((token_count + 1) * sizeof (char *));
	
	//Split the string up using strtok
	tokens[0] = strtok(src, del);
	for (int i = 1; i < token_count; i++) {
		char *token_start = strtok(NULL, delim);
		
		tokens[i] = token_start;
	}
	
	tokens[token_count] = NULL;
exit:
	//Put the number of tokens back into count if the user wants it
	if (count != NULL) {
		*count = token_count;
	}
	
	return tokens;	
}

//Reads a line from stdin, uses static allocation for strings
int stdin_read_line(char *str, unsigned int max_bytes)
{
    char *result = fgets(str, max_bytes, stdin);

    //If the return value of fgets was not the same as str, then something went wrong
    //return an error in that case.
    if (result != str) {
        return ERR_IO_READ_FAIL;
    }

    //Unless max_bytes have been read, remove the trailing newline.
    unsigned int bytes_read = strlen(str);
    if (bytes_read != max_bytes) {
        str[bytes_read - 1] = 0;
    }

    return SUCCESS;
}

/*char *stdin_prompt_line_dynamic(const char *prompt)
{
	printf(prompt);
	return stdin_read_line_dynamic();
}*/

int stdin_prompt_line(const char *prompt, char *str, unsigned int max_bytes)
{
    printf("%s", prompt);

    return stdin_read_line(str, max_bytes);
}
