#include "types.h"
#include "instruction.h"
#include "computer.h"

static int get_inst_type(ui32 inst)
{
    return (inst & 0xe0000000) >> 29;
}

i32 expand_imm(ui32 imm, int bit)
{
    if (bit > 31 || bit < 2)
        return imm;

    if (imm & (1 << (bit - 1)))
    {
        for (; bit < 32; ++bit)
            imm |= (1 << bit);

        return imm;
    }
    else
        return imm;
}

ui32 decode(struct n2_context *context)
{
    ui32 inst = N2PC_INST_TYPE_INVALID << 29;

    if (!context || context->pc == 0)
        return inst;

    inst = *(ui32 *)context->pc;
    context->pc += 4;

    return inst;
}

int execute(struct n2_computer *computer, ui32 inst)
{
    int ret = N2PC_ERR_SUCCESS;

    switch (get_inst_type(inst))
    {
    case N2PC_INST_TYPE_ARITHMETIC:
        ret = execute_arithmetic(computer, inst);
        break;
    case N2PC_INST_TYPE_MEMORY:
        ret = execute_memory(computer, inst);
        break;
    case N2PC_INST_TYPE_BRANCH:
        ret = execute_branch(computer, inst);
        break;
    case N2PC_INST_TYPE_MOVE:
        ret = execute_move(computer, inst);
        break;
    case N2PC_INST_TYPE_TEST:
        ret = execute_test(computer, inst);
        break;
    case N2PC_INST_TYPE_STACK:
        ret = execute_stack(computer, inst);
        break;
    case N2PC_INST_TYPE_SPECIAL: 
        ret = execute_special(computer, inst);
        break;
    default:
        ret = N2PC_ERR_INVALID_INSTRUCTION;
        break;
    }

    return ret;
}

int check_address_validity(struct n2_computer *computer, ui32 address)
{
    return
        ((address >= (ui32)computer->memory) && (((ui32)computer->memory + computer->memory_size) > address)) ||
        ((address >= (ui32)computer->stack) && (((ui32)computer->stack + computer->stack_size) > address));
}
