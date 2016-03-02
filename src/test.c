#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_opcode(ui32 inst)
{
    return (inst & 0x10000000) >> 28;
}

static int get_inst_imm(ui32 inst)
{
    return (inst & 0x8000000) >> 27;
}

static int get_inst_ra(ui32 inst)
{
    return (inst & 0x7000000) >> 24;
}

static int get_inst_rb(ui32 inst)
{
    return (inst & 0xe00000) >> 21;
}

static int get_inst_imm_operand(ui32 inst)
{
    return (inst & 0x1fffff);
}

static int test_tst(struct n2_context *context, ui32 left, ui32 right)
{
    ui32 result = left & right;

    if (!result)
        N2PC_STATUS_SET_Z(context->regs.status);
    else
        N2PC_STATUS_CLEAR_Z(context->regs.status);

    if (N2PC_IS_NEGATIVE(result))
        N2PC_STATUS_SET_N(context->regs.status);
    else
        N2PC_STATUS_CLEAR_N(context->regs.status);

    N2PC_STATUS_CLEAR_V(context->regs.status);
    N2PC_STATUS_CLEAR_C(context->regs.status);

    return N2PC_ERR_SUCCESS;
}

static int test_cmp(struct n2_context *context, ui32 left, ui32 right)
{
    int left_neg = N2PC_IS_NEGATIVE(left);
    int right_neg = N2PC_IS_NEGATIVE(right);

    ui32 result = left - right;

    int result_neg = N2PC_IS_NEGATIVE(result);

    if (!result)
        N2PC_STATUS_SET_Z(context->regs.status);
    else
        N2PC_STATUS_CLEAR_Z(context->regs.status);

    if (result_neg)
        N2PC_STATUS_SET_N(context->regs.status);
    else
        N2PC_STATUS_CLEAR_N(context->regs.status);

    if (left_neg == right_neg && result_neg != left_neg)
        N2PC_STATUS_SET_V(context->regs.status);
    else
        N2PC_STATUS_CLEAR_V(context->regs.status);

    if (result < left)
        N2PC_STATUS_SET_C(context->regs.status);
    else
        N2PC_STATUS_CLEAR_C(context->regs.status);

    return N2PC_ERR_SUCCESS;
}

int execute_test(struct n2_computer *computer, ui32 inst)
{
    ui32 left = computer->context.regs.nr[get_inst_ra(inst)];
    ui32 right = get_inst_imm(inst) ?
        expand_imm(get_inst_imm_operand(inst), 21) :
        computer->context.regs.nr[get_inst_rb(inst)];

    int err;

    switch (get_inst_opcode(inst))
    {
    case N2PC_OPCODE_TST:
        err = test_tst(&computer->context, left, right);
        break;
    case N2PC_OPCODE_CMP:
        err = test_cmp(&computer->context, left, right);
        break;
    default:
        err = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return err;
}