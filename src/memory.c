#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "memory.h"

#define _DBG_FILL_MEMORY

//Declared in memory.h

unsigned char cpu_mem[CPU_MEM_SIZE];
unsigned char *cpu_mem_base;

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
int cpu_mem_set(unsigned int offset, unsigned char value)
{
    if (IS_INVALID_ADDR(offset)) {
        return -ERR_MEM_OFFSET_INVALID;
    }

    *(cpu_mem_base + offset) = value;
    return SUCCESS;
}

//Sets an arbitrary number of bytes in the virtual memory from a specified array of integers
int cpu_mem_set_multiple(unsigned int offset, unsigned char *src, unsigned int size)
{
    if (IS_INVALID_ADDR(offset) || IS_INVALID_ADDR(offset + size)) {
        return -ERR_MEM_OFFSET_INVALID;
    }
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

int cpu_mem_read(unsigned char *value, unsigned int offset)
{
    if (IS_INVALID_ADDR(offset)) {
        return -ERR_MEM_OFFSET_INVALID;
    }

    *value = *(cpu_mem_base + offset);
    return SUCCESS;
}

int cpu_mem_read_multiple(unsigned char *dest, unsigned int offset, unsigned int size)
{
    if (IS_INVALID_ADDR(offset) || IS_INVALID_ADDR(offset + size)) {
        return -ERR_MEM_OFFSET_INVALID;
    }

    *dest = *(cpu_mem_base + offset);
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
        cpu_mem_set(i, value);
    }
#else
    memset(cpu_mem_base, 0, CPU_MEM_SIZE);
#endif
}




