#pragma once

#include "types.h"

/*
    * n2pc Instruction
        +------------------------------------------------+
        | type |                variable                 |
        +------------------------------------------------+
        * type[31:29] - determines type of instruction, can be one of following values
            * 000: Arithmetic Instruction
            * 001: Memory Instruction
            * 010: Branch Instruction
            * 011: Move Instruction
            * 100: Test Instruction
            * 101: Stack Instruction
            * 110: Special Instruction
            * 111: Invalid Instruction
        * variable[28:0] - has different formats for each instruction type

    * Arithmetic Instruction
        +----------------------------------------------------------+
        | 000 |  opcode  |  imm  |  rd  |  rs |      reserved      |
        +----------------------------------------------------------+
        * opcode[28:25] - arithmetic instructions can have one of following opcodes
            * add(0000) - performs addition
                if (imm == 1)
                    add rd, N       (rd <- rd + N)
                else
                    add rd, rs      (rd <- rd + rs)
            * sub(0001) - performs subtraction
                if (imm == 1)
                    sub rd, N       (rd <- rd - N)
                else
                    sub rd, rs      (rd <- rd - rs)
            * shl(0010) - performs left bit shift
                if (imm == 1)
                    shl rd, N       (rd <- rd << N)
                else
                    shl rd, rs      (rd <- rd << rs)
            * shr(0011) - performs right bit shift
                if (imm == 1)
                    shr rd, N       (rd <- rd >> N)
                else
                    shr rd, rs      (rd <- rd >> rs)
            * rol(0100) - performs left bit rotation
                if (imm == 1)
                    rol rd, N       (rd <- rd <<< N)
                else
                    rol rd, rs      (rd <- rd <<< rs)
            * ror(0101) - performs right bit rotation
                if (imm == 1)
                    ror rd, N       (rd <- rd >>> N)
                else
                    ror rd, rs      (rd <- rd >>> rs)
            * and(0110) - performs bitwise AND
                if (imm == 1)
                    and rd, N       (rd <- rd & N)
                else
                    and rd, rs      (rd <- rd & rs)
            * or(0111) - performs bitwise OR
                if (imm == 1)
                    or rd, N        (rd <- rd | N)
                else
                    or rd, rs       (rd <- rd | rs)
            * xor(1000) - performs bitwise XOR
                if (imm == 1)
                    xor rd, N       (rd <- rd ^ N)
                else
                    xor rd, rs      (rd <- rd ^ rs)
            * not(1001) - performs bitwise NOT
                    not rd          (rd <- ~rd)
        * imm[24] - if set, [20:0] is used as immediate operand
        * rd[23:21] - destination register
        * rs[20:18] - operand register
        * reserved[17:0] - must be zero

    * Memory Instruction
        +----------------------------------------------------------+
        | 001 |  opcode  |  rd  |  rs  |          offset           |
        +----------------------------------------------------------+
        * opcode[28] - memory instructions can have one of following opcodes
            * ldm(0) - loads a word from memory pointed by Rs to Rd
                ldm rd, rs, offset  (rd <- *(rs + offset))
            * stm(1) - stores a word to memory pointed by Rd from Rs
                stm rd, offset, rs  (*(rd + offset) <- rs)
        * rd[27:25] - destination register
        * rs[24:22] - source register
        * offset[21:0] - signed integer added to base memory address
    
    * Branch Instruction
        All branch instructions takes either register or immediate value for their operand.
        Next pc is calculated as follows:
            if (r == 1)
                jXX rs    (pc <- rs)
            else
                jXX N     (pc <- pc - 4 + (N * 4))
        +----------------------------------------------------------+
        | 010 |  opcode  |  r  |     rs[23:21] or offset[23:0]     |
        +----------------------------------------------------------+
        * opcode[28:25] - branch instructions can have one of following opcodes
            * jmp(0000) - branch unconditionally
            * jeq(0001) - branch if equal
                if (Z == 1)
                    branch
            * jneq(0010) - branch if not equal
                if (Z == 0)
                    branch
            * jgt(0011) - branch if greater (signed >)
                if (Z == 0 && N == V)
                    branch
            * jge(0100) - branch if greater or equal (signed >=)
                if (N == V)
                    branch
            * jlt(0101) - branch if less (signed <)
                if (N != V)
                    branch
            * jle(0110) - branch if less or equal (signed <=)
                if (Z == 1 || N != V)
                    branch
            * jhi(0111) - branch if higher (unsigned >)
                if (Z == 0 && C == 0)
                    branch
            * jhe(1000) - branch if higher or equal (unsigned >=)
                if (C == 0)
                    branch
            * jlo(1001) - branch if lower (unsigned <)
                if (C == 1)
                    branch
            * jloe(1010) - branch if lower or equal (unsigned <=)
                if (Z == 1 || C == 1)
                    branch
            * call(1011) - save current pc on stack and then branch
                push pc
                branch
            * ret(1100) - pop pc from stack memory
                ret
        * r[24] - if set, only [23:21] is used as register number
        * rs[23:21] - register which contains the address to branch
        * offset[23:0] - relative offset from current pc to branch
    
    * Move Instruction
        Move instruction has only a single instruction: mov
            mov rd, rs (rd <- rs)
        +----------------------------------------------------------+
        | 011 |  imm  |  rd  |  rs  |           reserved           |
        +----------------------------------------------------------+
        * imm[28] - if set, [24:0] is used as immediate operand
        * rd[27:25] - destination register
        * rs[24:22] - source register
        * reserved[21:0] - must be zero

    * Test Instruction
        +----------------------------------------------------------+
        | 100 |  opcode  |  imm  |  ra  |  rb  |     reserved      |
        +----------------------------------------------------------+
        * opcode[28] - test instructions can have one of following opcodes
            * tst(0) - perform logical and operation and discard result
            * cmp(1) - compare two operands
        * imm[27] - if set, [23:0] is used as immediate operand
        * ra[26:24] - first operand register
        * rb[23:21] - second operand register
        * reserved[20:0] - must be zero

    * Stack Instruction
        +----------------------------------------------------------+
        | 101 |  opcode  |  imm  |  rs  |         reserved         |
        +----------------------------------------------------------+
        * opcode[28:27] - stack instructions can have one of following opcodes
            * push(00) - push value onto stack memory
                if (imm == 1)
                    push N          (sp -= 4; *sp <- N)
                else
                    push rs         (sp -= 4; *sp <- rs)
            * pop(01) - pop value from stack memory
                pop rs              (rs = *sp; sp += 4)
            * lsp(10) - load current sp to register
                lsp rd              (rd <- sp)
            * ssp(11) - load register to current sp
                ssp rs              (sp <- rs)
        * imm[26] - if set, [26:0] is used as immediate operand (ignored when pop)
        * rs[25:23] - source register
        * reserved[22:0] - must be zero

    * Special Instruction
        +----------------------------------------------------------+
        | 110 |  opcode  |                 reserved                |
        +----------------------------------------------------------+
        * opcode[28:26] - special instructions can have one of following opcodes
            * open(000) - opens a file with given parameters in registers
                * in
                    nr1 - address of filename to open
                    nr2 - open mode (0: read / else: write)
                * out
                    nr0 - file descriptor
            * read(001) - reads from file via given file descriptor
                * in
                    nr1 - file descriptor
                    nr2 - address of buffer
                    nr3 - max size to read
                * out
                    nr0 - number of byte read
            * wrt(010) - writes to file via given file descriptor
                * in
                    nr1 - file descriptor
                    nr2 - address of buffer
                    nr3 - max size to write
                * out
                    nr0 - number of byte written
            * cls(011) - closes given file descriptor
                * in
                    nr1 - file descriptor
                * out
                    none
            * trm(100) - terminates program
                * in
                    none
                * out
                    none
        * reserved[25:0] - must be zero
 */

#define N2PC_MSB(val)           ((val) & (1 << 31))
#define N2PC_LSB(val)           ((val) & (1 <<  0))
#define N2PC_IS_NEGATIVE(val)   N2PC_MSB(val)

/* Instruction Types */
#define N2PC_INST_TYPE_ARITHMETIC      0
#define N2PC_INST_TYPE_MEMORY          1
#define N2PC_INST_TYPE_BRANCH          2
#define N2PC_INST_TYPE_MOVE            3
#define N2PC_INST_TYPE_TEST            4
#define N2PC_INST_TYPE_STACK           5
#define N2PC_INST_TYPE_SPECIAL         6
#define N2PC_INST_TYPE_INVALID         7

/* Arithmetic Instruction opcodes */
#define N2PC_OPCODE_ADD     0
#define N2PC_OPCODE_SUB     1
#define N2PC_OPCODE_SHL     2
#define N2PC_OPCODE_SHR     3
#define N2PC_OPCODE_ROL     4
#define N2PC_OPCODE_ROR     5
#define N2PC_OPCODE_AND     6
#define N2PC_OPCODE_OR      7
#define N2PC_OPCODE_XOR     8
#define N2PC_OPCODE_NOT     9

/* Memory Instruction opcodes */
#define N2PC_OPCODE_LDM     0
#define N2PC_OPCODE_STM     1

/* Branch Instruction opcodes */
#define N2PC_OPCODE_JMP     0
#define N2PC_OPCODE_JEQ     1
#define N2PC_OPCODE_JNEQ    2
#define N2PC_OPCODE_JGT     3
#define N2PC_OPCODE_JGE     4
#define N2PC_OPCODE_JLT     5
#define N2PC_OPCODE_JLE     6
#define N2PC_OPCODE_JHI     7
#define N2PC_OPCODE_JHE     8
#define N2PC_OPCODE_JLO     9
#define N2PC_OPCODE_JLOE    10
#define N2PC_OPCODE_CALL    11
#define N2PC_OPCODE_RET     12

/* Test Instruction opcodes */
#define N2PC_OPCODE_TST     0
#define N2PC_OPCODE_CMP     1

/* Stack Instruction opcodes */
#define N2PC_OPCODE_PUSH    0
#define N2PC_OPCODE_POP     1
#define N2PC_OPCODE_LSP     2
#define N2PC_OPCODE_SSP     3

/* Special Instruction opcodes */
#define N2PC_OPCODE_OPEN    0
#define N2PC_OPCODE_READ    1
#define N2PC_OPCODE_WRT     2
#define N2PC_OPCODE_CLS     3
#define N2PC_OPCODE_TRM     4

extern ui32 decode(struct n2_context *context);
extern int execute(struct n2_computer *computer, ui32 inst);
extern i32 expand_imm(ui32 imm, int bit);
extern int check_address_validity(struct n2_computer *computer, ui32 address);

/* arithmetic.c */
extern int execute_arithmetic(struct n2_computer *computer, ui32 inst);

/* memory.c */
extern int execute_memory(struct n2_computer *computer, ui32 inst);

/* branch.c */
extern int execute_branch(struct n2_computer *computer, ui32 inst);

/* move.c */
extern int execute_move(struct n2_computer *computer, ui32 inst);

/* test.c */
extern int execute_test(struct n2_computer *computer, ui32 inst);

/* stack.c */
extern int execute_stack(struct n2_computer *computer, ui32 inst);

/* special.c */
extern int execute_special(struct n2_computer *computer, ui32 inst);
