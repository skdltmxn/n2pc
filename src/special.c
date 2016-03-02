#include <stdio.h>
#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_opcode(ui32 inst)
{
    return (inst & 0x1c000000) >> 26;
}

static int special_open(struct n2_computer *computer)
{
    if (!check_address_validity(computer, computer->context.regs.nr[1]))
    {
        return N2PC_ERR_ACCESS_VIOLATION;
    }

    if (computer->context.regs.nr[2])
        computer->context.regs.nr[0] =
            (ui32)fopen((const char *)computer->context.regs.nr[1], "wb");
    else
        computer->context.regs.nr[0] =
            (ui32)fopen((const char *)computer->context.regs.nr[1], "rb");

    return N2PC_ERR_SUCCESS;
}

static int special_read(struct n2_computer *computer)
{
    FILE *fp = (FILE *)computer->context.regs.nr[1];
    if ((ui32)fp == 0)
        fp = stdin;

    computer->context.regs.nr[0] =
        fread((void *)computer->context.regs.nr[2], 1, computer->context.regs.nr[3], fp);

    return N2PC_ERR_SUCCESS;
}

static int special_wrt(struct n2_computer *computer)
{
    FILE *fp = (FILE *)computer->context.regs.nr[1];
    if ((ui32)fp == 1)
        fp = stdout;
    else if ((ui32)fp == 2)
        fp = stderr;

    computer->context.regs.nr[0] =
        fwrite((void *)computer->context.regs.nr[2], 1, computer->context.regs.nr[3], fp);

    return N2PC_ERR_SUCCESS;
}

static int special_cls(struct n2_computer *computer)
{
    FILE *fp = (FILE *)computer->context.regs.nr[1];
    if ((ui32)fp == 0)
        fp = stdin;
    else if ((ui32)fp == 1)
        fp = stdout;
    else if ((ui32)fp == 2)
        fp = stderr;

    fclose(fp);

    return N2PC_ERR_SUCCESS;
}

int execute_special(struct n2_computer *computer, ui32 inst)
{
    int err;

    switch (get_inst_opcode(inst))
    {
    case N2PC_OPCODE_OPEN:
        err = special_open(computer);
        break;
    case N2PC_OPCODE_READ:
        err = special_read(computer);
        break;
    case N2PC_OPCODE_WRT:
        err = special_wrt(computer);
        break;
    case N2PC_OPCODE_CLS:
        err = special_cls(computer);
        break;
    case N2PC_OPCODE_TRM:
        err = N2PC_ERR_PROGRAM_TERMINATED;
        break;
    default:
        err = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return err;
}
