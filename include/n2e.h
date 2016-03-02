#pragma once

#include "types.h"

/* n2e version */
#define N2E_VERSION     0x02

struct n2e
{
    ui8 version;
    ui32 size;
    byte *code;
};
