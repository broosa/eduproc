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

//Contains code for a (very) basic memory monitor
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "monitor.h"
#include "util.h"
#include "memory.h"

#define _CMD_SUCCESS 0
#define _CMD_ERR_SYNTAX -1
#define _CMD_ERR_ARG_COUNT -2
#define _CMD_ERR_INVALID_INT_LITERAL -3
#define _CMD_ERR_INVALID_BYTE_LITERAL -4
#define _CMD_ERR_OFFSET_INVALID -5

#define _DUMP_BYTES_PER_LINE 16

int _monitor_cmd_goto(char *argv[], int argc);
int _monitor_cmd_dump(char *argv[], int argc);
int _monitor_cmd_write(char *argv[], int argc);
int _monitor_cmd_exit(char *argv[], int argc);

unsigned int _parse_hex(char *str);
unsigned int _parse_int_arg(char *str);

void _display_dump(unsigned int offset, unsigned int size);

unsigned int monitor_offset;

int cmd_err;
int cmd_err_offset;
char *cmd_err_token;

void cpu_monitor_start(void)
{
    //Initalize the virtual memory space
    cpu_mem_init();

    monitor_offset = MONITOR_OFFSET_DEFAULT;

    //Set up a contextual prompt to show
    char prompt[12];

    char command[MONITOR_CMD_BUF_SIZE];
	
	//Main execution loop. This will eventually be shrunk down to size.
    while (1) {
        sprintf(prompt, "0x%08x> ", monitor_offset);
        stdin_prompt_line(prompt, command, MONITOR_CMD_BUF_SIZE);

        char *cmd_start = command;
        char cmd_char;
        
        //Clear the interpreter error statuses
        cmd_err_token = NULL;
        cmd_err_offset = 0;

        //Seek past any preceding spaces
        while (*cmd_start == ' ') {
            cmd_start++;
        }

        //Get the first non-whitespace character in the string. this is the command we want to run.
        cmd_char = *cmd_start;
        
        int token_count;
        char **cmd_tokens = split_args(command, &token_count);
        
        //Move on if the user didn't type anything in
        if (cmd_tokens == NULL) {
        	free(cmd_tokens);
        	continue;
        }
        
        //Separate out the command arguments
        int arg_count = token_count - 1;
        char **cmd_args = cmd_tokens + 1;
        
        int cmd_result = 0;
		
        if (cmd_char == 'g') {
			cmd_result = _monitor_cmd_goto(cmd_args, arg_count);
        } else if (cmd_char == 'd') {
			cmd_result = _monitor_cmd_dump(cmd_args, arg_count);
        } else if (cmd_char == 'w') {
            cmd_result = _monitor_cmd_write(cmd_args, arg_count);
        } else if (cmd_char == 'q') {
        	free(cmd_tokens);
            return;
        } else {
            printf("UNKOWN COMMAND\n");
        }
        
        free(cmd_tokens);
        
        if (cmd_result != _CMD_SUCCESS) {
        	if (cmd_result == _CMD_ERR_SYNTAX) {
        		printf("SYNTAX ERROR\n");
			} else if (cmd_result == _CMD_ERR_ARG_COUNT) {
				printf("INCORRECT NUMBER OF ARGUMENTS\n");
			} else if (cmd_result == _CMD_ERR_INVALID_INT_LITERAL) {
				printf("INVALID LITERAL FOR INT: %s\n", cmd_err_token);
			} else if (cmd_result == _CMD_ERR_INVALID_BYTE_LITERAL) {
				printf("INVALID LITERAL FOR BYTE: %s\n", cmd_err_token);
			} else if (cmd_result == _CMD_ERR_OFFSET_INVALID) {
				printf("INVALID MEMORY OFFSET: 0x%08x\n", cmd_err_offset);
			}
		}
    }

}

int _monitor_cmd_goto(char *argv[], int argc)
{
	if (argc != 1) {
		return _CMD_ERR_ARG_COUNT;
	}
	
	char *offset_token = argv[0];
    unsigned int new_offset = _parse_int_arg(offset_token);

    //Check to make sure that the address is valid (or that its a number at all)
    if (new_offset == 0) {
    	char *token = offset_token;
    	while (*token != 0) {
    		if (*token != '0' || *token != 'x') {
				cmd_err_token = argv[0];
				return _CMD_ERR_INVALID_INT_LITERAL;
			}
		}
    } else if (!IS_VALID_ADDR(new_offset)) {
        cmd_err_offset = new_offset;
        return _CMD_ERR_OFFSET_INVALID;
    }
    
    monitor_offset = new_offset;
    return _CMD_SUCCESS;
}

int _monitor_cmd_dump(char *argv[], int argc)
{
	if (argc != 1) {
		return _CMD_ERR_ARG_COUNT;
	}
	
	unsigned int dump_size = _parse_int_arg(argv[0]);
	
	if (dump_size == 0 && strcmp(argv[0], "0")) {
    	cmd_err_token = argv[0];
        return _CMD_ERR_INVALID_INT_LITERAL;
    }
	
	//If the end of the dump goes past the end of our virtual memory space, truncate the output
    if (monitor_offset + dump_size > CPU_MEM_SIZE) {
        dump_size = CPU_MEM_SIZE - monitor_offset;
    }
    _display_dump(monitor_offset, dump_size);
}

int _monitor_cmd_write(char *argv[], int argc)
{
	if (argc < 1) {
		return _CMD_ERR_ARG_COUNT;
	}
	
	unsigned int write_offset = monitor_offset;
	
	//Has the user specified a custom offset?
	char *first_token = argv[0];
	char *offset_token;
	
	int input_invalid = 0;
	
	if (first_token[0] == '@') {
		offset_token = first_token + 1;
		
		write_offset = _parse_int_arg(offset_token);
		
		if (write_offset == 0) {
			//Did the user really enter "0" or "0x000...0"?
			while (*first_token != 0) {
				if (*first_token != '0' && *first_token != 'x') {
					cmd_err_token = first_token;
					return _CMD_ERR_INVALID_INT_LITERAL;
				}
			}
		}
		
		if (IS_INVALID_ADDR(write_offset)) {
			cmd_err_offset = write_offset;
			return _CMD_ERR_OFFSET_INVALID;
		}
		
		//Increment argv so we can ignore the first token
		argv++;
		argc--;
	}
	
	//Prepare an array for the bytes we want to write
	unsigned char data_bytes[argc];
	
	for (int i = 0; i < argc; i++) {
		char *token = argv[i];
		
		//Give up if a token is larger than 3 characters
		if (strlen(token) > 2) {
			input_invalid = 1;
		}
		
		unsigned int token_value = _parse_hex(token);
		
		if (token_value == 0) {
			if (strcmp(token, "00") != 0 && strcmp(token, "0") != 0) {
  				input_invalid = 1;
  			}
  		}
  		
  		if (input_invalid) {
  			cmd_err_token = token;
  			return _CMD_ERR_INVALID_BYTE_LITERAL;
		}
		
		//Cast the byte to char. This might kill some significant digits
		//somewhere and should be handled better later.
		data_bytes[i] = (unsigned char) token_value;	
	}
	
	//Make sure that the memory manager does not write past the end of the emulated memory
	int write_size = 0;
	if (write_offset + argc > CPU_MEM_SIZE) {
		DEBUG_OUTPUT("DEBUG: Requested write goes out of bounds. Truncating.\n");
		write_size = CPU_MEM_SIZE - write_offset;
	} else {
		write_size = argc;
	}
	
	cpu_mem_set_multiple(write_offset, data_bytes, write_size);
}

int _monitor_cmd_exit(char **argv, int argc)
{
	//Cleanup will go in here later
}

void _display_dump(unsigned int offset, unsigned int size)
{
    for (int i = 0; i < size; i++) {
        unsigned int read_offset = offset + i;
        unsigned char mem_value;

        cpu_mem_read(&mem_value, read_offset);

        //At the beginning of the line, print the offset at the initial spot
        if (i % _DUMP_BYTES_PER_LINE == 0) {
            printf("0x%08x:", read_offset);
        }
        //print out the hex representation of the value at the current offset
        printf(" %02x", mem_value);

        //If we are at the end of a line, print a newline
        if (i % _DUMP_BYTES_PER_LINE == _DUMP_BYTES_PER_LINE - 1 || i == size - 1) {
            printf("\n");
        }
    }
}

//TODO: Handle bad input better (invalid literals)
unsigned int _parse_hex(char *str)
{
	return (unsigned int) strtoul(str, NULL, 16);
}

unsigned int _parse_int_arg(char *str)
{
    if (str[1] == 'x') {
        return (unsigned int) strtoul(str, NULL, 16);
    } else {
        return (unsigned int) strtoul(str, NULL, 10);
    }
}
