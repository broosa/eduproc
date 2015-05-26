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
#include <limits.h>

#include "cpu.h"
#include "memory.h"
#include "util.h"

cpu_reg_state saved_state;

unsigned int regs[CPU_NUM_REGISTERS] = {0};

unsigned char curr_instr_data[INSTR_LEN] = {0};

//Special use registers
unsigned int * const sp = &regs[SP_REG_INDEX];
unsigned int * const lr = &regs[LR_REG_INDEX];
unsigned int * const pc = &regs[PC_REG_INDEX];

//The status register is handled separately.
unsigned int sr = 0;

//Condition flags
unsigned char cnd_z = 0;
unsigned char cnd_c = 0;
unsigned char cnd_n = 0;
unsigned char cnd_v = 0;

inline unsigned int _unpack_word(unsigned char *buf);
unsigned int _map_mem_addr(unsigned char addr_mode, unsigned int r2,
                    unsigned int r3, unsigned int data);

void _cpu_do_op_arith(unsigned char op_code, unsigned char op_flags,
                    unsigned int *dest, unsigned int op1, unsigned int op2);

cpu_reg_state *cpu_get_last_reg_state(void)
{
	return &saved_state;
}

void _update_last_state() 
{
	memcpy(saved_state.regs, regs, CPU_NUM_REGISTERS * sizeof(unsigned int));
	saved_state.sp = *sp;
	saved_state.lr = *lr;
	saved_state.pc = *pc;
	
	saved_state.cnd_z = cnd_z;
	saved_state.cnd_c = cnd_c;
	saved_state.cnd_n = cnd_n;
	saved_state.cnd_v = cnd_v;

	memcpy(saved_state.last_instr, curr_instr_data, INSTR_LEN);
}

void cpu_do_step(void)
{
    unsigned int load_address = *pc;
    if (IS_INVALID_ADDR(load_address) ||
    		IS_INVALID_ADDR(load_address + INSTR_LEN)) {
        DEBUG_OUTPUT("Invalid PC address!\n");
        return;
    }
    
    //Load the next instruction from memory (8 bytes)
    cpu_mem_read_multiple(curr_instr_data, load_address, INSTR_LEN);
    
    //Extract the opcode
    //Op code is the first byte
    unsigned char op_code = curr_instr_data[0];
    unsigned char op_type = op_code & CPU_OPTYPE_MASK;
    
    //Extract the condition flags from the instruction
    //These are the 4 most significant bits of byte 1
    unsigned char exec_cond = curr_instr_data[1] >> 4;
    
    //The last 4 bits of byte 1 are the op flags. We AND the byte
    //with 0x0f to get this value
    unsigned char op_flags = curr_instr_data[1] & 0x0f;
    unsigned char op_set_status_bit = op_flags >> 3;
    
    unsigned char op_addr_mode = curr_instr_data[2] >> 4;
    
    unsigned char op_r1_num = curr_instr_data[2] & 0x0f;
    unsigned char op_r2_num = curr_instr_data[3] >> 4;
    unsigned char op_r3_num = curr_instr_data[3] & 0x0f;
    
    //Unpack the 32 bit data field
    unsigned int op_data = _unpack_word(curr_instr_data + 4);
    
    unsigned int *op_r1 = &regs[op_r1_num];
    unsigned int *op_r2 = &regs[op_r2_num];
    unsigned int *op_r3 = &regs[op_r3_num];
    
    unsigned int data_addr = 0;
    unsigned int data;

    if (op_type == CPU_OPTYPE_SYSTEM) {
        
    } else if (op_type == CPU_OPTYPE_MEM) {
        //Handle jump to location in memory
        if (op_code == CPU_OP_JMP) {
        	unsigned int dest_addr = 0;
        	if (BIT_SET(op_addr_mode, ADDR_MODE_BIT_IND)) {
				dest_addr = _map_mem_addr(op_addr_mode, *op_r1, *op_r2,
								op_data);
        	} else {
        		dest_addr = op_data;
        	}
            if (OPFLAG_SET(op_flags, OPFLAG_MEM_SET_LINK)) {
                //Set the link register to the address of the next instruction
                *lr = *pc + INSTR_LEN;
            }
            
            if (IS_INVALID_ADDR(dest_addr)) {
            	DEBUG_OUTPUT("Invalid jump address: 0x%08x\n", dest_addr);
            	*pc += INSTR_LEN;
            	return;
            }
            *pc = dest_addr;
            _update_last_state();
            return;
        //If we've the destination or source address of data is stored
        //in a register, calculate that address.
        } else if (BIT_SET(op_addr_mode, ADDR_MODE_BIT_IND)) {
            data_addr = _map_mem_addr(op_addr_mode, *op_r2, *op_r3,
                            op_data);
        }
                        
        //Do something with the computed location in memory (load, store)
        unsigned int data;
        switch (op_code) {
        
        case CPU_OP_STR:

        	//If we aren't storing to an address stored in a register,
        	//then data_addr shouldn't have been set already. We set it
        	//ourselves.
        	if (!BIT_SET(op_addr_mode, ADDR_MODE_BIT_IND)) {
        		data_addr = op_data;
        	}

            data = *op_r1;
            cpu_mem_write_byte(data_addr, data);
            break;
        case CPU_OP_LDR:
        	if (!BIT_SET(op_addr_mode, ADDR_MODE_BIT_IND)) {
        		data = op_data;
        	} else {
            	data = cpu_mem_read_word(&data, data_addr);
        	}
            *op_r1 = data;
            break;
        default:
        	DEBUG_OUTPUT("Unknown opcode: 0x%02x\n", op_code);
			break;
        }
    } else if (op_type == CPU_OPTYPE_ARITH) {
        unsigned int op1 = *op_r2;
        
        unsigned int op2 = 0;
        if (OPFLAG_SET(op_flags, OPFLAG_ARITH_IMD)) {
            op2 = op_data;
        } else {
            op2 = *op_r3;
        }
        
        unsigned int result;
        
        _cpu_do_op_arith(op_code, op_flags, &result, op1, op2);
        *op_r1 = result;
    } else if (op_type == CPU_OPTYPE_BIT_ARITH) {
    } else if (op_type == CPU_OPTYPE_COMPARE) {
    }
    //Update the program counter
    *pc += INSTR_LEN;
    _update_last_state();
}   

void cpu_init(void)
{
	//Program counter is initially set to the address 0x8000
	//Stack originally starts at 0x4000
    *pc = 0x00008000;
    *lr = 0x00000000;
	*sp = 0x00004000;

	cnd_c = 0;
	cnd_v = 0;
	cnd_z = 0;
	cnd_n = 0;

	_update_last_state();
}

void cpu_reset(void) 
{
	//TODO: Figure out what should actually happen on a cpu reset
	cpu_init();
}

//TODO: Make a structure to encapsulate condition flags, rather than
//using global variables
void _cpu_do_op_arith(unsigned char op_code, unsigned char op_flags, 
                    unsigned int *dest, unsigned int op1, unsigned int op2)
{
    unsigned int result;
    switch (op_code) {
    case CPU_OP_ADD:
        if (op2 > UINT_MAX - op1) {
            if (OPFLAG_SET(op_flags, OPFLAG_SETS_STATUS)) {
                cnd_c = 1;
            }
        }
    
        result = op1 + op2;
        break;
    case CPU_OP_SUB:
        result = op2 - op1;
        break;
    case CPU_OP_RSUB:
        result = op1 - op2;
        break;
    case CPU_OP_MUL:
        if (op2 > UINT_MAX / op1) {
            if (OPFLAG_SET(op_flags, OPFLAG_SETS_STATUS)) {
                cnd_c = 1;
            }
        }
        result = op1 * op2;
        break;
    case CPU_OP_DIV:
    	if (op2 == 0) {
    		DEBUG_OUTPUT("Divide by 0!\n");
    		break;
    	}

    	result = op2 / op1;
    	break;
    }
    
    if (OPFLAG_SET(op_flags, OPFLAG_SETS_STATUS)) {
        cnd_z = ~(result & 1);
        cnd_n = ~(result >> 31);
        //A signed overflow occurred (both operands had the same sign
        //bit but the result had a different sign bit
        cnd_v = ((op1 >> 31) == (op2 >> 31)) && ((op1 >> 31) != (result >> 31));  
    }

    *dest = result;
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
unsigned int _unpack_word(unsigned char *buf)
{
/*#if CPU_BIG_ENDIAN
    return ((unsigned int) buf[0] << 24) | ((unsigned int) buf[1] << 16)
                    | ((unsigned int) buf[2] << 8)
                    | ((unsigned int) buf[3]);
#else*/
    return ((unsigned int) buf[0]) | ((unsigned int) buf[1] << 8)
                    | ((unsigned int) buf[2] << 16)
                    | ((unsigned int) buf[3] << 24);
//#endif
}
