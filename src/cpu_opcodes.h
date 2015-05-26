#ifndef _CPU_OPCODES_H_INCLUDED
#define _CPU_OPCODES_H_INCLUDED

#define CPU_OPTYPE_MASK 0xf0

#define CPU_OPTYPE_SYSTEM 0x00
#define CPU_OPTYPE_MEM 0x10
#define CPU_OPTYPE_ARITH 0x20
#define CPU_OPTYPE_BIT_ARITH 0x30
#define CPU_OPTYPE_COMPARE 0x40

#define OPFLAG_SETS_STATUS 3

	//No Operation
#define CPU_OP_NOP 0x01
#define CPU_OP_INT 0x02
//MEMORY OPERATIONS

//Jump
#define CPU_OP_JMP 0x10

//Memory Store, Word
#define CPU_OP_STR 0x11
#define CPU_OP_STM 0x12

//Memory Store, Unsigned Halfword
#define CPU_OP_STRH 0x13
//Memory Store, Signed Halfword
#define CPU_OP_STRSH 0x14

//Memory Store, Unsigned Byte
#define CPU_OP_STRB 0x15
//Memory Store, Signed Byte
#define CPU_OP_STRSB 0x16

//Memory Load, Word
#define CPU_OP_LDR 0x17
#define CPU_OP_LDM 0x18

//Memory Load, Unsigned Halfword
#define CPU_OP_LDRH 0x19
//Memory Load, Signed Halfword
#define CPU_OP_LDRSH 0x1a

//Memory Load, Unsigned Byte
#define CPU_OP_LDRB 0x1b
//Memory Load, Signed Byte
#define CPU_OP_LDRSB 0x1c

//Memory Move
#define CPU_OP_MOV 0x15

//ARITHMETIC OPERATIONS
#define CPU_OP_ADD 0x20

#define CPU_OP_SUB 0x21
#define CPU_OP_RSUB 0x22

#define CPU_OP_MUL 0x23
#define CPU_OP_DIV 0x24

//Shift left / right
#define CPU_OP_SL 0x30
#define CPU_OP_SR 0x31

//Rotate left / right
#define CPU_OP_ROL 0x32
#define CPU_OP_ROR 0x33

#define CPU_OP_AND 0x34
#define CPU_OP_OR 0x35

#define CPU_OP_XOR 0x36

#define CPU_OP_NEG 0x37

//COMPARISON OPERATIONS
#define CPU_OP_CMP 0x40
#define CPU_OP_TST 0x41

//Memory instruction opflags
#define OPFLAG_MEM_PROC 2
#define OPFLAG_MEM_INC 1
#define OPFLAG_MEM_POST 0

//If low bit of opflags is set, JMP will set the link register
#define OPFLAG_MEM_SET_LINK 0

//Arithmetic opflags
#define OPFLAG_ARITH_IMD 1
#define OPFLAG_ARITH_CARRY 0

//ADDRESS MODE BITS
//Is the data indirect?
#define ADDR_MODE_BIT_IND 3
//Is the address of the data stored in a register, or is it in the instruction?
#define ADDR_MODE_BIT_IND_REG 2
//Is the data at an offset from the specified address?
#define ADDR_MODE_BIT_OFFSET 1
//Is the offset in a register or in the instruction?
#define ADDR_MODE_BIT_OFFSET_REG 0

#endif
