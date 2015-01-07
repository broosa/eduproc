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

#define _DUMP_BYTES_PER_LINE 16

unsigned int _parse_int_arg(char *str);
void _display_dump(unsigned int offset, unsigned int size);

unsigned int monitor_offset;

void cpu_monitor_start(void)
{
    //Initalize the virtual memory space
    cpu_mem_init();

    monitor_offset = MONITOR_OFFSET_DEFAULT;

    //Set up a contextual prompt to show
    char prompt[12];

    char command[MONITOR_CMD_BUF_SIZE];

    while (1) {
        sprintf(prompt, "0x%08x> ", monitor_offset);
        stdin_prompt_line(prompt, command, MONITOR_CMD_BUF_SIZE);

        char *cmd_start = command;
        char cmd_char;

        //Seek past any preceding spaces
        while (*cmd_start == ' ') {
            cmd_start++;
        }

        //Get the first non-whitespace character in the string. this is the command we want to run.
        cmd_char = *cmd_start;

        if (cmd_char == 'g') {
            //Skip the first token (this is the command string, we want the arguments only)
            strtok(command, " ");

            char *offset_str = strtok(NULL, " ");
            unsigned int new_offset = _parse_int_arg(offset_str);

            //Check to make sure that the address is valid (or that its a number at all)
            if (!IS_VALID_ADDR(new_offset)) {
                printf("INVALID OFFSET: 0x%08x", new_offset);
                break;
            } else if (new_offset == 0 && strcmp(offset_str, "0") != 0) {
                printf("SYNTAX ERROR");
                break;
            }

            monitor_offset = new_offset;
            break;
        } else if (cmd_char == 'd') {
            strtok(command, " ");

            char *dump_size_str = strtok(NULL, " ");

            int dump_size = 0;
            //If the string is null, then there was a syntax error
            if (dump_size_str != NULL) {
                dump_size = _parse_int_arg(dump_size_str);
            } else {
                printf("SYNTAX ERROR");
                break;
            }

            if (dump_size == 0) {
                printf("INVALID DUMP SIZE");
                break;
            }

            //If the end of the dump goes past the end of our virtual memory space, truncate the output
            if (monitor_offset + dump_size > CPU_MEM_SIZE) {
                dump_size = CPU_MEM_SIZE - monitor_offset;
            }
            _display_dump(monitor_offset, dump_size);
            break;

        } else if (cmd_char == 'w') {

            char *cmd_str_copy = malloc(strlen(command) + 1);
            strcpy(cmd_str_copy, command);
            printf("%s", cmd_str_copy);

            //Discard the first token of the copied string
            strtok(cmd_str_copy, " ");
            int arg_count = 0;

            //Count the number of arguments
            while (strtok(NULL, " ") != NULL) {
                arg_count++;
            }

            free(cmd_str_copy);

            DEBUG_OUTPUT("DEBUG: Monitor found %d tokens for w command", arg_count);

            strtok(command, " ");
        } else if (cmd_char == 'q') {
            return;
        } else {
            printf("SYNTAX ERROR");
        }

    }

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
            printf("
");
        }
    }
}

unsigned int _parse_int_arg(char *str)
{
    if (str[1] == 'x') {
        return (unsigned int) strtoul(str, NULL, 16);
    } else {
        return (unsigned int) strtoul(str, NULL, 10);
    }
}
