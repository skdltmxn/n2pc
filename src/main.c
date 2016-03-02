#include <stdio.h>
#include <stdlib.h>
#include "computer.h"

static const char *exception_message(int err)
{
    switch (err)
    {
    case N2PC_ERR_INVALID_COMPUTER:
        return "Invalid computer";
    case N2PC_ERR_INVALID_EXECUTALBLE:
        return "Invalid executable";
    case N2PC_ERR_INVALID_INSTRUCTION:
        return "Invalid instruction";
    case N2PC_ERR_ACCESS_VIOLATION:
        return "Access violation";
    default:
        return "Unknown exception";
    }
}

static void usage(const char *self)
{
    printf("[Usage] %s <n2e> [args]...\n", self);
    printf("  n2e - executable for n2pc\n");
    printf("  args - arguments for n2e (optional)\n\n");
    printf("[Ex]\n");
    printf("  %s myprogram.n2e arg1 arg2 arg3\n", self);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        usage(argv[0]);
        return 1;
    }
    
    struct n2_computer computer;

    /* 16K memory, 4K stack */
    init_computer(&computer, 16 * 1024, 4 * 1024);

    ++argv;
    computer.n2e_name = *argv++;

    if (argc > 2)
    {
        int input_count = argc - 2;

        computer.argc = input_count;
        computer.argv = (char **)malloc(sizeof(char *) * input_count);
        for (int i = 0; i < input_count; ++i)
            computer.argv[i] = *argv++;
    }

    int err = run_computer(&computer);

    if (computer.exception.faulted)
        printf("SYSTEM FAULT[0x%08x] - %s\n", computer.exception.err,
            exception_message(computer.exception.err));

    shutdown_computer(&computer);

    return 0;
}