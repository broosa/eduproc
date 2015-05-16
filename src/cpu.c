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
#include <string.h>
#include <assert.h>

#include "cpu.h"
#include "cpu_opcodes.h"

#define INSTR_LEN 8

#define BIG_ENDIAN 0
 
cpu_reg_state * const saved_state;

unsigned int regs[NUM_REGISTERS] = {0};

unsigned char curr_instr_data[INSTR_LEN] = {0};

//Special use registers
unsigned int * const sp = &regs[SP_REG_INDEX];
unsigned int * const lr = &regs[LR_REG_INDEX];
unsigned int * const pc = &regs[LR_REG_INDEX];

//The status register is handled separately.
unsigned int sr = 0;

//Condition flags
unsigned char cnd_z = 0;
unsigned char cnd_c = 0;
unsigned char cnd_n = 0;
unsigned char cnd_v = 0;

inline unsigned int _unpack_int(unsigned char *buf);
unsigned int _map_mem_addr(unsigned char addr_mode, unsigned int r2,
                    unsigned int r3, unsigned int data);

cpu_reg_state *cpu_get_last_reg_state(void)
{
	return saved_state;
}

void _update_last_state() 
{
	memcpy(saved_state->regs, regs, NUM_REGISTERS);
	saved_state->sp = *sp;
	saved_state->lr = *lr;
	saved_state->pc = *pc;
	
	saved_state->cnd_z = cnd_z;
	saved_state->cnd_c = cnd_c;
	saved_state->cnd_n = cnd_n;
	saved_state->cnd_v = cnd_v;	
}

void cpu_do_step(void)
{
    unsigned int *load_offset = *pc;
    if (IS_INVALID_ADDR(load_address)) {
        DEBUG_OUTPUT("Invalid PC address!");
        return;
    }
    
    //Load the next instruction from memory (8 bytes)
    cpu_mem_read_multiple(instr_data, load_offset, INSTR_LEN);
    
    //Extract the opcode
    //Op code is the first byte
    unsigned char op_code = instr_data[0]
    unsigned char op_type = opcode & CPU_OPTYPE_MASK;
    
    //Extract the condition flags from the instruction
    //These are the 4 most significant bits of byte 1
    unsigned char exec_cond = instr_data[1] >> 4;
    
    //The last 4 bits of byte 1 are the op flags. We AND the byte
    //with 0x0f to get this value
    unsigned char op_flags = instr_data[1] & 0x0f
    unsigned char op_set_status_bit = op_flags >> 3;
    
    unsigned char op_addr_mode = instr_data[2] >> 4;
    
    unsigned char op_r1_num = instr_data[2] & 0x0f;
    unsigned char op_r2_num = instr_data[3] >> 4;
    unsigned char op_r3_num = instr_data[3] & 0x0f;
    
    //Unpack the 32 bit data field
    unsigned int op_data = _unpack_int(instr_data + 4);
    
    unsigned int *op_r1 = &regs[op_r1_num];
    unsigned int *op_r2 = &regs[op_r3_num];
    unsigned int *op_r3 = &regs[op_r4_num];
    
    if (op_type == CPU_OPTYPE_SYSTEM) {
        
    } else if (op_type == CPU_OPTYPE_MEM) {
        //Handle jump to location in memory
        if (op_code == CPU_OP_JMP) {
            unsigned int dest_addr = _map_mem_addr(op_addr_mode, *op_r1, *op_r2,
                            op_data);
            if (BIT_SET(op_flags, OPFLAG_BIT_MEM_SET_LINK)) {
                //Set the link register to the address of the next instruction
                *lr = *pc + INSTR_LEN;
            }
            
            *pc = dest_addr;
            return;
        }
        unsigned int data_addr = _map_mem_addr(op_addr_mode, *op_r2, *op_r3,
                        op_data);
                        
        //Do something with the computed location in memory (load, store)
        switch (op_code) {
        
        case CPU_OP_STR:
        default:
        }
    } else if (op_type == CPU_OPTYPE_ARITH) {
    } else if (op_type == CPU_OPTYPE_BIT_ARITH) {
    } else if (op_type == CPU_OPTYPE_COMPARE) {
    }
    load_offset += 8
}   

void cpu_init(void)
{
    
}

void cpu_reset(void) 
{

}

//Computes the final address of data in memory given a certain address mode
//and base/offset registers
unsigned int _map_mem_addr(unsigned char addr_mode, unsigned int r2,
                    unsigned int r3, unsigned int data)
{
    //Make sure that we are mapping when the data is indirectly addressed
    //We shouldn't map when the data is in the instruction
    assert(BIT_SET(addr_mode, ADDR_MODE_BIT_IND));
    
    //Is the base address stored in r2?
    if (BIT_SET(addr_mode, ADDR_MODE_BIT_IND_REG)) {
        unsigned int base_addr = r2;
        
        //Is the data at an offset from the address in r2?
        if (BIT_SET(addr_mode, ADDR_MODE_BIT_OFFSET)) {
            //Is the offset stored in r3?
            if (BIT_SET(addr_mode, ADDR_MODE_BIT_OFFSET_REG)) {
                return base_addr + r3;
            } else {
                return base_addr + data;
            }
        } else {
            return base_addr;
        }
    } else {
        //The address is stored in the data field
        return data;
    }
        
}

//Unpacks an integer from an array of unsigned chars
//Accounts on whether emulated processor is big or little endian
inline unsigned int _unpack_int(unsigned char *buf)
{
#if BIG_ENDIAN
    return ((unsigned int) buf[0] << 24) & ((unsigned int) buf[1] << 16) 
                    & ((unsigned int) buf[2] << 8) 
                    & ((unsigned int) buf[3]);
#else
    return ((unsigned int) buf[0]) & ((unsigned int) buf[1] << 8) 
                    & ((unsigned int) buf[2] << 16) 
                    & ((unsigned int) buf[3] << 24);                
#endif
}
