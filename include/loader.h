#pragma once

#include "n2e.h"

/* errors */
#define N2PC_ERR_BASE_LOADER           (N2PC_ERR_BASE - 200)
#define N2PC_ERR_INVALID_N2E           (N2PC_ERR_BASE_LOADER - 1)
#define N2PC_ERR_INVALID_VERSION       (N2PC_ERR_BASE_LOADER - 2)

extern int load_n2e(const byte *in, ui32 size, struct n2e *out);
extern void unload_n2e(struct n2e *in);
