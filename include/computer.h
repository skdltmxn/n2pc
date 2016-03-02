#pragma once

#include "types.h"

/* errors */
#define N2PC_ERR_BASE_COMPUTER         (N2PC_ERR_BASE - 100)
#define N2PC_ERR_INVALID_COMPUTER      (N2PC_ERR_BASE_COMPUTER - 1)
#define N2PC_ERR_INVALID_EXECUTALBLE   (N2PC_ERR_BASE_COMPUTER - 2)
#define N2PC_ERR_INVALID_INSTRUCTION   (N2PC_ERR_BASE_COMPUTER - 3)
#define N2PC_ERR_ACCESS_VIOLATION      (N2PC_ERR_BASE_COMPUTER - 4)
#define N2PC_ERR_PROGRAM_TERMINATED    (N2PC_ERR_BASE_COMPUTER - 5)
//#define N2PC_ERR_EXCEPTION             (N2PC_ERR_BASE_COMPUTER - 6)

#define N2PC_MINIMUM_COMPUTER_MEMORY   (4 * 1024)  /* min 4K memory */
#define N2PC_MAXIMUM_COMPUTER_MEMORY   (32 * 1024) /* max 32K memory */
#define N2PC_MINIMUM_COMPUTER_STACK    (1 * 1024)  /* min 1K stack */
#define N2PC_MAXIMUM_COMPUTER_STACK    (8 * 1024)  /* max 8K stack */

/*
    * n2pc Registers
        * nr0~7: general purpose registers

        * status: contains status of current context in following format
        +----------------------------------------------------------+
        |                       reserved           | N | V | C | Z |
        +----------------------------------------------------------+
            * reserved[31:1] - must be zero
            * Z[0] - Zero bit
            * C[1] - Carry bit
            * V[2] - Overflow bit
            * N[3] - Negative bit
        
        * pc: program counter
        * sp: stack pointer
*/
#define N2PC_STATUS_FLAG_Z      (1 << 0)
#define N2PC_STATUS_FLAG_C      (1 << 1)
#define N2PC_STATUS_FLAG_V      (1 << 2)
#define N2PC_STATUS_FLAG_N      (1 << 3)

#define N2PC_STATUS_SET_FLAG(status, flag)      (status |= flag)
#define N2PC_STATUS_CLEAR_FLAG(status, flag)    (status &= ~flag)
#define N2PC_STATUS_ISSET_FLAG(status, flag)    (status & flag)
#define N2PC_STATUS_SET_Z(status)               N2PC_STATUS_SET_FLAG(status, N2PC_STATUS_FLAG_Z)
#define N2PC_STATUS_SET_C(status)               N2PC_STATUS_SET_FLAG(status, N2PC_STATUS_FLAG_C)
#define N2PC_STATUS_SET_V(status)               N2PC_STATUS_SET_FLAG(status, N2PC_STATUS_FLAG_V)
#define N2PC_STATUS_SET_N(status)               N2PC_STATUS_SET_FLAG(status, N2PC_STATUS_FLAG_N)
#define N2PC_STATUS_CLEAR_Z(status)             N2PC_STATUS_CLEAR_FLAG(status, N2PC_STATUS_FLAG_Z)
#define N2PC_STATUS_CLEAR_C(status)             N2PC_STATUS_CLEAR_FLAG(status, N2PC_STATUS_FLAG_C)
#define N2PC_STATUS_CLEAR_V(status)             N2PC_STATUS_CLEAR_FLAG(status, N2PC_STATUS_FLAG_V)
#define N2PC_STATUS_CLEAR_N(status)             N2PC_STATUS_CLEAR_FLAG(status, N2PC_STATUS_FLAG_N)
#define N2PC_STATUS_ISSET_Z(status)             N2PC_STATUS_ISSET_FLAG(status, N2PC_STATUS_FLAG_Z)
#define N2PC_STATUS_ISSET_C(status)             N2PC_STATUS_ISSET_FLAG(status, N2PC_STATUS_FLAG_C)
#define N2PC_STATUS_ISSET_V(status)             N2PC_STATUS_ISSET_FLAG(status, N2PC_STATUS_FLAG_V)
#define N2PC_STATUS_ISSET_N(status)             N2PC_STATUS_ISSET_FLAG(status, N2PC_STATUS_FLAG_N)

struct n2_regs
{
    ui32 nr[8]; /* nr0 ~ nr7 */
    ui32 status;
};

struct n2_context
{
    ui32 pc;
    ui32 sp;
    struct n2_regs regs;
};

struct n2_computer
{
    char *n2e_name;
    int argc;
    char **argv;
    struct n2_context context;
    byte *memory;
    ui32 memory_size;
    byte *stack;
    ui32 stack_size;
    struct {
        int faulted;
        int err;
    } exception;
};

extern void init_computer(struct n2_computer *computer, ui32 memory_size, ui32 stack_size);
extern void shutdown_computer(struct n2_computer *computer);
extern int run_computer(struct n2_computer *computer);
extern void set_exception(struct n2_computer *computer, int err);
