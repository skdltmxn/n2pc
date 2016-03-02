#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_opcode(ui32 inst)
{
    return (inst & 0x1e000000) >> 25;
}

static int get_inst_imm(ui32 inst)
{
    return (inst & 0x1000000) >> 24;
}

static int get_inst_rd(ui32 inst)
{
    return (inst & 0xe00000) >> 21;
}

static int get_inst_rs(ui32 inst)
{
    return (inst & 0x1c0000) >> 18;
}

static int get_inst_imm_operand(ui32 inst)
{
    return (inst & 0x1fffff);
}

static int arithmetic_add(struct n2_context *context, int rd, ui32 left, ui32 right)
{
    int left_neg = N2PC_IS_NEGATIVE(left);
    int right_neg = N2PC_IS_NEGATIVE(right);

    ui32 result = left + right;
    context->regs.nr[rd] = result;

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

static int arithmetic_sub(struct n2_context *context, int rd, ui32 left, ui32 right)
{
    return arithmetic_add(context, rd, left, ~right + 1);
}

static int arithmetic_shl(struct n2_context *context, int rd, ui32 left, ui32 count)
{
    ui32 masked_count = count & 0x1f;
    int carry = 0;

    while (masked_count--)
    {
        carry = N2PC_MSB(left);
        left <<= 1;
    }

    context->regs.nr[rd] = left;

    if (!left)
        N2PC_STATUS_SET_Z(context->regs.status);
    else
        N2PC_STATUS_CLEAR_Z(context->regs.status);

    if (N2PC_IS_NEGATIVE(left))
        N2PC_STATUS_SET_N(context->regs.status);
    else
        N2PC_STATUS_CLEAR_N(context->regs.status);

    if ((count & 0x1f) == 1)
    {
        if (N2PC_MSB(left) ^ carry)
            N2PC_STATUS_SET_V(context->regs.status);
        else
            N2PC_STATUS_CLEAR_V(context->regs.status);
    }

    if (carry)
        N2PC_STATUS_SET_C(context->regs.status);
    else
        N2PC_STATUS_CLEAR_C(context->regs.status);

    return N2PC_ERR_SUCCESS;
}

static int arithmetic_shr(struct n2_context *context, int rd, ui32 left, ui32 count)
{
    ui32 left_save = left;
    ui32 masked_count = count & 0x1f;
    int carry = 0;

    while (masked_count--)
    {
        carry = N2PC_LSB(left);
        left /= 2;
    }

    context->regs.nr[rd] = left;

    if (!left)
        N2PC_STATUS_SET_Z(context->regs.status);
    else
        N2PC_STATUS_CLEAR_Z(context->regs.status);

    if (N2PC_IS_NEGATIVE(left))
        N2PC_STATUS_SET_N(context->regs.status);
    else
        N2PC_STATUS_CLEAR_N(context->regs.status);

    if ((count & 0x1f) == 1)
    {
        if (N2PC_MSB(left_save))
            N2PC_STATUS_SET_V(context->regs.status);
        else
            N2PC_STATUS_CLEAR_V(context->regs.status);
    }

    if (carry)
        N2PC_STATUS_SET_C(context->regs.status);
    else
        N2PC_STATUS_CLEAR_C(context->regs.status);

    return N2PC_ERR_SUCCESS;
}

static int arithmetic_rol(struct n2_context *context, int rd, ui32 left, ui32 count)
{
    ui32 masked_count = count & 0x1f;
    int carry = 0;

    while (masked_count--)
    {
        int msb = N2PC_MSB(left);
        left = (left << 1) + msb;
    }

    context->regs.nr[rd] = left;

    if (!left)
        N2PC_STATUS_SET_Z(context->regs.status);
    else
        N2PC_STATUS_CLEAR_Z(context->regs.status);

    if (N2PC_IS_NEGATIVE(left))
        N2PC_STATUS_SET_N(context->regs.status);
    else
        N2PC_STATUS_CLEAR_N(context->regs.status);

    carry = N2PC_LSB(left);

    if ((count & 0x1f) == 1)
    {
        if (N2PC_MSB(left) ^ carry)
            N2PC_STATUS_SET_V(context->regs.status);
        else
            N2PC_STATUS_CLEAR_V(context->regs.status);
    }

    if (carry)
        N2PC_STATUS_SET_C(context->regs.status);
    else
        N2PC_STATUS_CLEAR_C(context->regs.status);

    return N2PC_ERR_SUCCESS;
}

static int arithmetic_ror(struct n2_context *context, int rd, ui32 left, ui32 count)
{
    ui32 masked_count = count & 0x1f;
    int carry = 0;

    while (masked_count--)
    {
        int lsb = N2PC_LSB(left);
        left = (left >> 1) + (lsb << 31);
    }

    context->regs.nr[rd] = left;

    if (!left)
        N2PC_STATUS_SET_Z(context->regs.status);
    else
        N2PC_STATUS_CLEAR_Z(context->regs.status);

    if (N2PC_IS_NEGATIVE(left))
        N2PC_STATUS_SET_N(context->regs.status);
    else
        N2PC_STATUS_CLEAR_N(context->regs.status);

    carry = N2PC_MSB(left);

    if ((count & 0x1f) == 1)
    {
        if (N2PC_MSB(left) ^ (left & (1 << 30)))
            N2PC_STATUS_SET_V(context->regs.status);
        else
            N2PC_STATUS_CLEAR_V(context->regs.status);
    }

    if (carry)
        N2PC_STATUS_SET_C(context->regs.status);
    else
        N2PC_STATUS_CLEAR_C(context->regs.status);

    return N2PC_ERR_SUCCESS;
}

static int arithmetic_and(struct n2_context *context, int rd, ui32 left, ui32 right)
{
    ui32 result = left & right;
    context->regs.nr[rd] = result;

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

static int arithmetic_or(struct n2_context *context, int rd, ui32 left, ui32 right)
{
    ui32 result = left | right;
    context->regs.nr[rd] = result;

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

static int arithmetic_xor(struct n2_context *context, int rd, ui32 left, ui32 right)
{
    ui32 result = left ^ right;
    context->regs.nr[rd] = result;

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

static int arithmetic_not(struct n2_context *context, int rd, ui32 val)
{
    context->regs.nr[rd] = ~val;
    
    return N2PC_ERR_SUCCESS;
}

int execute_arithmetic(struct n2_computer *computer, ui32 inst)
{
    struct n2_context *context = &computer->context;
    int rd = get_inst_rd(inst);
    ui32 left = context->regs.nr[rd];
    ui32 right = get_inst_imm(inst) ?
        expand_imm(get_inst_imm_operand(inst), 21) :
        context->regs.nr[get_inst_rs(inst)];
    int err;

    switch (get_inst_opcode(inst))
    {
    case N2PC_OPCODE_ADD:
        err = arithmetic_add(context, rd, left, right);
        break;
    case N2PC_OPCODE_SUB:
        err = arithmetic_sub(context, rd, left, right);
        break;
    case N2PC_OPCODE_SHL:
        err = arithmetic_shl(context, rd, left, right);
        break;
    case N2PC_OPCODE_SHR:
        err = arithmetic_shr(context, rd, left, right);
        break;
    case N2PC_OPCODE_ROL:
        err = arithmetic_rol(context, rd, left, right);
        break;
    case N2PC_OPCODE_ROR:
        err = arithmetic_ror(context, rd, left, right);
        break;
    case N2PC_OPCODE_AND:
        err = arithmetic_and(context, rd, left, right);
        break;
    case N2PC_OPCODE_OR:
        err = arithmetic_or(context, rd, left, right);
        break;
    case N2PC_OPCODE_XOR:
        err = arithmetic_xor(context, rd, left, right);
        break;
    case N2PC_OPCODE_NOT:
        err = arithmetic_not(context, rd, left);
        break;
    default:
        err = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return err;
}
