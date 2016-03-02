#pragma once

#pragma warning(disable:4996)

typedef unsigned char     byte;
typedef unsigned char     ui8;
typedef unsigned short    ui16;
typedef unsigned int      ui32;
typedef unsigned __int64  ui64;

typedef char              i8;
typedef short             i16;
typedef int               i32;
typedef __int64           i64;

#define N2PC_ERR_SUCCESS    0
#define N2PC_ERR_BASE       (-1000)
#define N2PC_ERR_OUTOFMEM   (N2PC_ERR_BASE - 1)