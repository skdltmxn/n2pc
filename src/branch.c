#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_opcode(ui32 inst)
{
    return (inst & 0x1e000000) >> 25;
}

static int get_inst_r(ui32 inst)
{
    return (inst & 0x1000000) >> 24;
}

static int get_inst_rs(ui32 inst)
{
    return (inst & 0xe00000) >> 21;
}

static int get_inst_offset(ui32 inst)
{
    return (inst & 0xffffff);
}

static int branch_jmp(struct n2_context *context, int r, ui32 offset)
{
    if (r)
        context->pc = offset;
    else
        context->pc = context->pc - 4 + (offset << 2);

    return N2PC_ERR_SUCCESS;
}

static int branch_jeq(struct n2_context *context, int r, ui32 offset)
{
    if (N2PC_STATUS_ISSET_Z(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jneq(struct n2_context *context, int r, ui32 offset)
{
    if (!N2PC_STATUS_ISSET_Z(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jgt(struct n2_context *context, int r, ui32 offset)
{
    if (!N2PC_STATUS_ISSET_Z(context->regs.status) &&
        N2PC_STATUS_ISSET_N(context->regs.status) == N2PC_STATUS_ISSET_V(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jge(struct n2_context *context, int r, ui32 offset)
{
    if (N2PC_STATUS_ISSET_N(context->regs.status) == N2PC_STATUS_ISSET_V(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jlt(struct n2_context *context, int r, ui32 offset)
{
    if (N2PC_STATUS_ISSET_N(context->regs.status) != N2PC_STATUS_ISSET_V(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jle(struct n2_context *context, int r, ui32 offset)
{
    if (N2PC_STATUS_ISSET_Z(context->regs.status) ||
        N2PC_STATUS_ISSET_N(context->regs.status) != N2PC_STATUS_ISSET_V(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jhi(struct n2_context *context, int r, ui32 offset)
{
    if (!N2PC_STATUS_ISSET_Z(context->regs.status) && !N2PC_STATUS_ISSET_C(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jhe(struct n2_context *context, int r, ui32 offset)
{
    if (!N2PC_STATUS_ISSET_C(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jlo(struct n2_context *context, int r, ui32 offset)
{
    if (N2PC_STATUS_ISSET_C(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_jloe(struct n2_context *context, int r, ui32 offset)
{
    if (N2PC_STATUS_ISSET_Z(context->regs.status) || N2PC_STATUS_ISSET_C(context->regs.status))
        branch_jmp(context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_call(struct n2_computer *computer, int r, ui32 offset)
{
    if (computer->context.sp - 4 < (ui32)computer->stack)
    {
        return N2PC_ERR_ACCESS_VIOLATION;
    }

    computer->context.sp -= 4;
    *(ui32 *)computer->context.sp = computer->context.pc;

    branch_jmp(&computer->context, r, offset);

    return N2PC_ERR_SUCCESS;
}

static int branch_ret(struct n2_computer *computer)
{
    if (computer->context.sp + 4 > (ui32)computer->stack + computer->stack_size)
    {
        return N2PC_ERR_ACCESS_VIOLATION;
    }

    computer->context.pc = *(ui32 *)computer->context.sp;
    computer->context.sp += 4;

    return N2PC_ERR_SUCCESS;
}

int execute_branch(struct n2_computer *computer, ui32 inst)
{
    int r = get_inst_r(inst);
    ui32 offset = r ?
        computer->context.regs.nr[get_inst_rs(inst)] :
        expand_imm(get_inst_offset(inst), 24);
    int err;

    switch (get_inst_opcode(inst))
    {
    case N2PC_OPCODE_JMP:
        err = branch_jmp(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JEQ:
        err = branch_jeq(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JNEQ:
        err = branch_jneq(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JGT:
        err = branch_jgt(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JGE:
        err = branch_jge(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JLT:
        err = branch_jlt(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JLE:
        err = branch_jle(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JHI:
        err = branch_jhi(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JHE:
        err = branch_jhe(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JLO:
        err = branch_jlo(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_JLOE:
        err = branch_jloe(&computer->context, r, offset);
        break;
    case N2PC_OPCODE_CALL:
        err = branch_call(computer, r, offset);
        break;
    case N2PC_OPCODE_RET:
        err = branch_ret(computer);
        break;
    default:
        err = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return err;
}