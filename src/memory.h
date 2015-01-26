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

#ifndef _MEMORY_H_INCLUDED
#define _MEMORY_H_INCLUDED

//CPU Available Memory (KB)
#define CPU_MEM_SIZE_KB 1024
#define CPU_MEM_SIZE CPU_MEM_SIZE_KB * 1024

#define SUCCESS 0
#define ERR_MEM_SEGFAULT 1
#define ERR_MEM_OFFSET_INVALID 2
#define ERR_MEM_COPY_OVERLAP 3
#define ERR_MEM_FILE_IO 4
#define ERR_MEM_FILE_TOO_BIG 5

#define IS_VALID_ADDR(offset) (offset < CPU_MEM_SIZE)
#define IS_INVALID_ADDR(offset) !IS_VALID_ADDR(offset)

#define CPU_MEM_PTR(offset) cpu_mem_base + offset

extern unsigned char *cpu_mem_base;

extern int cpu_mem_errno;

int cpu_mem_get_ptr(unsigned char *ptr, unsigned int offset);

int cpu_mem_set(unsigned int offset, unsigned char value);
int cpu_mem_set_multiple(unsigned int offset, unsigned char *src, unsigned int size);

int cpu_mem_copy(unsigned int dst_offset, unsigned int src_offset, unsigned int size);

int cpu_mem_read(unsigned char *value, unsigned int offset);
int cpu_mem_read_multiple(unsigned char *dest, unsigned int offset, unsigned int size);

int cpu_mem_load_file(char *filename, int offset);

void cpu_mem_init();
void cpu_mem_clear();
#endif
