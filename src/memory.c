#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_opcode(ui32 inst)
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

static int get_inst_offset(ui32 inst)
{
    return (inst & 0x1fffff);
}

static int memory_ldm(struct n2_computer *computer, int rd, int rs, i32 offset)
{
    ui32 base = computer->context.regs.nr[rs];

    if (!check_address_validity(computer, base))
        return N2PC_ERR_ACCESS_VIOLATION;

    computer->context.regs.nr[rd] = *(ui32 *)(base + offset);

    return N2PC_ERR_SUCCESS;
}

static int memory_stm(struct n2_computer *computer, int rd, int rs, i32 offset)
{
    ui32 base = computer->context.regs.nr[rd];

    if (!check_address_validity(computer, base))
        return N2PC_ERR_ACCESS_VIOLATION;

    *(ui32 *)(base + offset) = computer->context.regs.nr[rs];

    return N2PC_ERR_SUCCESS;
}

int execute_memory(struct n2_computer *computer, ui32 inst)
{
    int rd = get_inst_rd(inst);
    int rs = get_inst_rs(inst);
    i32 offset = expand_imm(get_inst_offset(inst), 22);
    int err;

    switch (get_inst_opcode(inst))
    {
    case N2PC_OPCODE_LDM:
        err = memory_ldm(computer, rd, rs, offset);
        break;
    case N2PC_OPCODE_STM:
        err = memory_stm(computer, rd, rs, offset);
        break;
    default:
        err = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return err;
}
