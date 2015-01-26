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

#include "cpu.h"
 
cpu_reg_state * const saved_state;

unsigned int regs[NUM_REGISTERS] = {0};

unsigned int * const sp = &regs[SP_REG_INDEX];
unsigned int * const lr = &regs[LR_REG_INDEX];
unsigned int * const pc = &regs[LR_REG_INDEX];

//The status register is handled separately.
unsigned int sr = 0;

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
}