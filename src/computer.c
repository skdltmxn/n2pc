#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "computer.h"
#include "n2e.h"
#include "loader.h"
#include "instruction.h"

static int execution_loop(struct n2_computer *computer, struct n2e *n2e)
{
    /* setup initial context */
    computer->context.pc = (ui32)n2e->code;
    computer->context.sp = (ui32)computer->stack + computer->stack_size;
    computer->context.regs.nr[0] = (ui32)computer->memory;
    computer->context.regs.nr[1] = computer->argc;

    /* write program inputs to memory */
    if (computer->argc)
    {
        if (!computer->argv)
        {
            set_exception(computer, N2PC_ERR_INVALID_EXECUTALBLE);
            return N2PC_ERR_INVALID_EXECUTALBLE;
        }

        char *p = computer->memory;
        for (int i = computer->argc - 1; i >= 0; --i)
        {
            size_t len = strlen(computer->argv[i]);
            memcpy(p, computer->argv[i], len + 1);
            computer->context.sp -= 4;
            *(ui32 *)computer->context.sp = (ui32)p;
            p += len + 1;
        }
    }

    int err = N2PC_ERR_SUCCESS;
    while (1)
    {
        if (computer->context.pc < (ui32)n2e->code ||
            computer->context.pc > (ui32)n2e->code + n2e->size)
        {
            set_exception(computer, N2PC_ERR_ACCESS_VIOLATION);
            return N2PC_ERR_ACCESS_VIOLATION;
        }

        ui32 inst = decode(&computer->context);
        err = execute(computer, inst);

        if (err == N2PC_ERR_PROGRAM_TERMINATED)
            return err;
        else if (err != N2PC_ERR_SUCCESS)
        {
            set_exception(computer, err);
            return err;
        }
    }

    return N2PC_ERR_SUCCESS;
}

void init_computer(struct n2_computer *computer, ui32 memory_size, ui32 stack_size)
{
    if (computer)
    {
        computer->n2e_name = 0;
        computer->argc = 0;
        computer->argv = 0;
        memset(&computer->context, 0, sizeof(struct n2_context));

        if (memory_size < N2PC_MINIMUM_COMPUTER_MEMORY)
            memory_size = N2PC_MINIMUM_COMPUTER_MEMORY;
        else if (memory_size > N2PC_MAXIMUM_COMPUTER_MEMORY)
            memory_size = N2PC_MAXIMUM_COMPUTER_MEMORY;

        if (stack_size < N2PC_MINIMUM_COMPUTER_STACK)
            stack_size = N2PC_MINIMUM_COMPUTER_STACK;
        else if (stack_size > N2PC_MAXIMUM_COMPUTER_STACK)
            stack_size = N2PC_MAXIMUM_COMPUTER_STACK;

        computer->memory = (byte *)malloc(memory_size);
        computer->memory_size = memory_size;
        computer->stack = (byte *)malloc(stack_size);
        computer->stack_size = stack_size;

        computer->exception.faulted = 0;
        computer->exception.err = N2PC_ERR_SUCCESS;
    }
}

void shutdown_computer(struct n2_computer *computer)
{
    if (computer)
    {
        if (computer->argc) free(computer->argv);
        if (computer->memory) free(computer->memory);
        if (computer->stack) free(computer->stack);
    }
}

int run_computer(struct n2_computer *computer)
{
    if (!computer)
        return N2PC_ERR_INVALID_COMPUTER;

    if (!computer->n2e_name)
    {
        set_exception(computer, N2PC_ERR_INVALID_EXECUTALBLE);
        return N2PC_ERR_INVALID_EXECUTALBLE;
    }

    FILE *fp = fopen(computer->n2e_name, "rb");
    if (!fp)
    {
        set_exception(computer, N2PC_ERR_INVALID_EXECUTALBLE);
        return N2PC_ERR_INVALID_EXECUTALBLE;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    byte *buffer = (byte *)malloc(size);

    if (!buffer)
    {
        fclose(fp);
        set_exception(computer, N2PC_ERR_OUTOFMEM);
        return N2PC_ERR_OUTOFMEM;
    }

    fread(buffer, size, 1, fp);
    fclose(fp);

    struct n2e n2e;
    int err = load_n2e(buffer, size, &n2e);
    free(buffer);

    if (err != N2PC_ERR_SUCCESS)
    {
        set_exception(computer, err);
        unload_n2e(&n2e);
        return err;
    }

    execution_loop(computer, &n2e);

    unload_n2e(&n2e);

    return N2PC_ERR_SUCCESS;
}

void set_exception(struct n2_computer *computer, int err)
{
    computer->exception.err = err;
    computer->exception.faulted = 1;
}
