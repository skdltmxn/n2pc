#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_imm(ui32 inst)
{
    return (inst & 0x10000000) >> 28;
}

static int get_inst_rd(ui32 inst)
{
    return (inst & 0xe000000) >> 25;
}

static int get_inst_rs(ui32 inst)
{
    return (inst & 0x1c00000) >> 22;
}

static int get_inst_imm_operand(ui32 inst)
{
    return (inst & 0x1ffffff);
}

static int mov(struct n2_context *context, int rd, ui32 source)
{
    context->regs.nr[rd] = source;
    return N2PC_ERR_SUCCESS;
}

int execute_move(struct n2_computer *computer, ui32 inst)
{
    ui32 source = get_inst_imm(inst) ?
        expand_imm(get_inst_imm_operand(inst), 25) :
        computer->context.regs.nr[get_inst_rs(inst)];

    return mov(&computer->context, get_inst_rd(inst), source);
}