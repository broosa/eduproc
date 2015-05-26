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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

#include "util.h"
#include "memory.h"

#define FILE_IO_BLOCK_SIZE 1024

//#define _DBG_FILL_MEMORY

//Declared in memory.h

unsigned char cpu_mem[CPU_MEM_SIZE];
unsigned char *cpu_mem_base;

int cpu_mem_errno;

inline unsigned int _unpack_word(unsigned char *buf);
inline unsigned int _pack_word(unsigned char *buf, unsigned int value);

//Returns a pointer to the given offset in memory

int cpu_mem_get_ptr(unsigned char *ptr, unsigned int offset)
{
    if (IS_INVALID_ADDR(offset)) {
        return -ERR_MEM_OFFSET_INVALID;
    }

    ptr = CPU_MEM_PTR(offset);
    return SUCCESS;
}

//Sets the value of the byte at address offset
int cpu_mem_write_byte(unsigned int offset, unsigned char value)
{
    if (IS_INVALID_ADDR(offset)) {
        return ERR_MEM_OFFSET_INVALID;
    }

    *(cpu_mem_base + offset) = value;
    return -SUCCESS;
}

//Sets an arbitrary number of bytes in the virtual memory from a specified array of integers
int cpu_mem_write_multiple(unsigned int offset, unsigned char *src, unsigned int size)
{
    if (IS_INVALID_ADDR(offset) || IS_INVALID_ADDR(offset + size)) {
        return -ERR_MEM_OFFSET_INVALID;
    }
    
    char *dest = cpu_mem_base + offset;
    
    if (!memcpy(dest, src, size)) {
    	return -ERR_MEM_SEGFAULT;
    }
    return SUCCESS;
}

//Copies an arbitrary number of bytes from src_offset to dst_offset
int cpu_mem_copy(unsigned int dst_offset, unsigned int src_offset, unsigned int size)
{
    //Check if the memory regions overlap
    if (IS_VALID_ADDR(src_offset) && IS_VALID_ADDR(dst_offset) && IS_VALID_ADDR(src_offset + size) &&
        IS_VALID_ADDR(dst_offset + size)) {
        //Make sure none of the destination memory locations are outside the range of memory
        if (src_offset + size >= dst_offset || dst_offset + size >= src_offset) {
            return -ERR_MEM_COPY_OVERLAP;
        } else {
            unsigned char *src_ptr = cpu_mem_base + src_offset;
            unsigned char *dst_ptr = cpu_mem_base + dst_offset;

            if (!memcpy(dst_ptr, src_ptr, size)) {
                return -ERR_MEM_SEGFAULT;
            }
            return SUCCESS;
        }
    } else {
        return -ERR_MEM_OFFSET_INVALID;
    }
}

int cpu_mem_read_byte(unsigned char *value, unsigned int offset)
{
    if (IS_INVALID_ADDR(offset)) {
        return -ERR_MEM_OFFSET_INVALID;
    }

    *value = *(cpu_mem_base + offset);
    return SUCCESS;
}

int cpu_mem_read_word(unsigned int *dest, unsigned int offset) {

	if (IS_INVALID_ADDR(offset) || IS_INVALID_ADDR(offset + MEM_WORD_SIZE)) {
		return -ERR_MEM_OFFSET_INVALID;
	}

	unsigned char *word_base = CPU_MEM_PTR(offset);
	*dest = _unpack_word(word_base);

	return SUCCESS;
}

int cpu_mem_read_multiple(unsigned char *dest, unsigned int offset, unsigned int size)
{
    if (IS_INVALID_ADDR(offset) || IS_INVALID_ADDR(offset + size)) {
        return -ERR_MEM_OFFSET_INVALID;
    }

    memcpy(dest, CPU_MEM_PTR(offset), size * sizeof(unsigned char));
    return SUCCESS;
}

//Loads the contents of the given filename into memory at the given offset (if space permits)
int cpu_mem_load_file(char *filename, int offset)
{
	//Opens the file for reading
	FILE *file_ptr = fopen(filename, "r");
	if (file_ptr == NULL) {
		cpu_mem_errno = errno;
		return -ERR_MEM_FILE_IO;
	}
	
	//Find the size of the file
	fseek(file_ptr, 0L, SEEK_END);
	int file_size = ftell(file_ptr);
	//Seek back to the beginning of the file
	fseek(file_ptr, 0L, SEEK_SET);
	
	if (IS_INVALID_ADDR(offset + file_size)) {
		fclose(file_ptr);
		return -ERR_MEM_FILE_TOO_BIG;
	}
	
	//Set up a buffer for future file io
	unsigned char data[FILE_IO_BLOCK_SIZE] = {0};
	
	
	int bytes_read = fread(data, sizeof(char), FILE_IO_BLOCK_SIZE, file_ptr);

	while (bytes_read != 0) {

		int write_result = cpu_mem_write_multiple(offset, data, bytes_read);
		bytes_read = fread(data, sizeof(char), FILE_IO_BLOCK_SIZE, file_ptr);

		if (write_result != SUCCESS) {
			DEBUG_OUTPUT("WARNING: IO FAILED!\n");
			fclose(file_ptr);
			return write_result;
		}
	}
	fclose(file_ptr);
	return SUCCESS;
}		
	
void cpu_mem_init()
{
    cpu_mem_base = &cpu_mem[0];

#ifdef _DBG_FILL_MEMORY
    //For debugging purposes, fill the memory with random values.
    //We use a constant seed so values don't change accross runs, but still are random.
    srand(123456);

    for (int i = 0; i < CPU_MEM_SIZE; i++) {
        unsigned char value = (unsigned char) rand();
        cpu_mem_write_byte(cpu_mem_base + i, value);
    }
#else
    memset(cpu_mem_base, 0, CPU_MEM_SIZE);
#endif
}

//Unpacks an integer from an array of unsigned chars
//Accounts on whether emulated processor is big or little endian
inline unsigned int _unpack_word(unsigned char *buf)
{
#if MEM_BIG_ENDIAN
    return ((unsigned int) buf[0] << 24) & ((unsigned int) buf[1] << 16)
                    & ((unsigned int) buf[2] << 8)
                    & ((unsigned int) buf[3]);
#else
    return ((unsigned int) buf[0]) & ((unsigned int) buf[1] << 8)
                    & ((unsigned int) buf[2] << 16)
                    & ((unsigned int) buf[3] << 24);
#endif
}

inline unsigned int _pack_word(unsigned char *buf, unsigned int value)
{
	for (int i = 0; i < MEM_WORD_SIZE; i++) {
#if MEM_BIG_ENDIAN
		buf[i] = (unsigned char) ((value >> ((3 - i) * 8)) & 0xff);
#else
		buf[i] = (unsigned char) ((value >> (i * 8)) & 0xff);
#endif
	}
}
