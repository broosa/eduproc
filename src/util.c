#include <stdio.h>
#include <string.h>

#include "util.h"	

//Reads a line from stdin, uses dynamic allocation for strings (Returns char *)
char *stdin_read_line_dynamic(void)
{
	char *dst = NULL;
	size_t bytes_allocated = 0;
	unsigned int bytes_read = getline(&dst, &bytes_allocated, stdin);
	return dst;
}
	
//Reads a line from stdin, uses static allocation for strings
int stdin_read_line(char *str, unsigned int max_bytes)
{	
	char *result = fgets(str, max_bytes, stdin);
	
	//If the return value of fgets was not the same as str, then something went wrong
	//return an error in that case.
	if (result != str)
	{
		return ERR_IO_READ_FAIL;
	}
	
	//Unless max_bytes have been read, remove the trailing newline. 
	unsigned int bytes_read = strlen(str);
	if (bytes_read != max_bytes) 
	{
		str[bytes_read - 1] = 0;
	}

	return SUCCESS;
}

char *stdin_prompt_line_dynamic(const char *prompt)
{
	printf(prompt);
	return stdin_read_line_dynamic();
}

int stdin_prompt_line(const char *prompt, char *str, unsigned int max_bytes)
{
	printf(prompt);
	
	return stdin_read_line(str, max_bytes);
}


		 
