#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "types.h"
#include "loader.h"

int load_n2e(const byte *in, ui32 size, struct n2e *out)
{
    if (!in || !out)
        return N2PC_ERR_INVALID_N2E;

    /* version check */
    if (*in != N2E_VERSION)
        return N2PC_ERR_INVALID_VERSION;

    out->version = *in++;
    out->size = *(ui32 *)in; in += 4;
    out->code = (byte *)malloc(out->size);
    if (!out->code)
        return N2PC_ERR_OUTOFMEM;

    memcpy(out->code, in, out->size);

    return N2PC_ERR_SUCCESS;
}

void unload_n2e(struct n2e *n2e)
{
    if (n2e && n2e->code)
        free(n2e->code);
}
