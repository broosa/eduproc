/*
*
* This file is a main executable to be used for testing purposes (can be modified
* to test the functionality of various individual modules
*
*/


#include <stdio.h>
#include <string.h>

#include "util.h"

int main(void)
{
    char line[100];

    while (1) {
        //line = "";
        int read_success = stdin_prompt_line("> ", line, 100);
        printf("User entered %d characters: %s\n", strlen(line), line);

        if (strcmp(line, "quit") == 0) {
            printf("Quitting...\n");
            break;
        }
    }
    return 0;
}
