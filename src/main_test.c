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
        printf("User entered %d characters: %s
               ", strlen(line), line);

        if (strcmp(line, "quit") == 0) {
            printf("Quitting...
                   ");
            break;
        }
    }
    return 0;
}
