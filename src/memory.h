#ifndef _MEMORY_H_INCLUDED
#define _MEMORY_H_INCLUDED

//CPU Available Memory (KB)
#define CPU_MEM_SIZE_KB 1024
#define CPU_MEM_SIZE CPU_MEM_SIZE_KB * 1024

#define SUCCESS 0
#define ERR_MEM_SEGFAULT 1
#define ERR_MEM_OFFSET_INVALID 2
#define ERR_MEM_COPY_OVERLAP 3

#define IS_VALID_ADDR(offset) (offset < CPU_MEM_SIZE)
#define IS_INVALID_ADDR(offset) !IS_VALID_ADDR(offset)

#define CPU_MEM_PTR(offset) cpu_mem_base + offset

extern unsigned char *cpu_mem_base;

int cpu_mem_get_ptr(unsigned char *ptr, unsigned int offset);

int cpu_mem_set(unsigned int offset, unsigned char value);
int cpu_mem_set_multiple(unsigned int offset, unsigned char *src, unsigned int size);

int cpu_mem_copy(unsigned int dst_offset, unsigned int src_offset, unsigned int size);

int cpu_mem_read(unsigned char *value, unsigned int offset);
int cpu_mem_read_multiple(unsigned char *dest, unsigned int offset, unsigned int size);

void cpu_mem_init();
void cpu_mem_clear();
#endif
