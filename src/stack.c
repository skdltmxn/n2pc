#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_opcode(ui32 inst)
{
    return (inst & 0x18000000) >> 27;
}

static int get_inst_imm(ui32 inst)
{
    return (inst & 0x4000000) >> 26;
}

static int get_inst_rs(ui32 inst)
{
    return (inst & 0x3800000) >> 23;
}

static int get_inst_imm_operand(ui32 inst)
{
    return (inst & 0x3ffffff);
}

static int stack_push(struct n2_computer *computer, ui32 source)
{
    if (computer->context.sp - 4 < (ui32)computer->stack)
    {
        return N2PC_ERR_ACCESS_VIOLATION;
    }

    computer->context.sp -= 4;
    *(ui32 *)computer->context.sp = source;

    return N2PC_ERR_SUCCESS;
}

static int stack_pop(struct n2_computer *computer, int rd)
{
    if (computer->context.sp + 4 > (ui32)computer->stack + computer->stack_size)
    {
        return N2PC_ERR_ACCESS_VIOLATION;
    }

    computer->context.regs.nr[rd] = *(ui32 *)computer->context.sp;
    computer->context.sp += 4;

    return N2PC_ERR_SUCCESS;
}

static int stack_lsp(struct n2_computer *computer, int rd)
{
    computer->context.regs.nr[rd] = computer->context.sp;

    return N2PC_ERR_SUCCESS;
}

static int stack_ssp(struct n2_computer *computer, int rs)
{
    computer->context.sp = computer->context.regs.nr[rs];

    return N2PC_ERR_SUCCESS;
}

int execute_stack(struct n2_computer *computer, ui32 inst)
{
    ui32 source = get_inst_imm(inst) ?
        expand_imm(get_inst_imm_operand(inst), 26) :
        computer->context.regs.nr[get_inst_rs(inst)];

    int err;

    switch (get_inst_opcode(inst))
    {
    case N2PC_OPCODE_PUSH:
        err = stack_push(computer, source);
        break;
    case N2PC_OPCODE_POP:
        err = stack_pop(computer, get_inst_rs(inst));
        break;
    case N2PC_OPCODE_LSP:
        err = stack_lsp(computer, get_inst_rs(inst));
        break;
    case N2PC_OPCODE_SSP:
        err = stack_ssp(computer, get_inst_rs(inst));
        break;
    default:
        err = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return err;
}
