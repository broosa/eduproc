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

#ifndef _CPU_H_INCLUDED
#define _CPU_H_INCLUDED

#include "cpu_opcodes.h"

#define CPU_NUM_REGISTERS 16

//Each instruction is 64 bits (8 Bytes) long
#define INSTR_LEN 8

#define _CPU_BIG_ENDIAN 0

#define SP_REG_INDEX 13
#define LR_REG_INDEX 14
#define PC_REG_INDEX 15

#define CPU_ARCH_RISC 1
#define CPU_START_ADDR 0x8000

//Find whether the nth least signicant bit in x is set
#define BIT_SET(x, n) ((x) & (1 << (n)))
#define OPFLAG_SET(op_flags, flags) BIT_SET(op_flags, flags)

typedef struct {
    unsigned int regs[CPU_NUM_REGISTERS];
    unsigned int pc;
    unsigned int lr;
    unsigned int sp;
    unsigned int sr;

    unsigned short int cnd_z;
    unsigned short int cnd_c;
    unsigned short int cnd_n;
    unsigned short int cnd_v;

    unsigned char last_instr[INSTR_LEN];
} cpu_reg_state;

extern cpu_reg_state * const last_state;

cpu_reg_state *cpu_get_last_reg_state(void);

void cpu_init(void);
void cpu_reset(void);

void cpu_do_step(void);
#endif
