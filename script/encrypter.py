# -*- coding: utf-8 -*-
'''
    Flag: computers_CAN_run_Another_Computer!
'''

from register import *
from arithmetic import *
from branch import *
from label import *
from memory import *
from move import *
from test import *
from stack import *
from special import *
from program import *

p = Program([
    Cmp(nr1, 2),     # nr1 contains number of args
    Jge('has_args'),
    Label('fail'),
    Mov(nr0, 0x4c49),
    Shl(nr0, 16),
    Or(nr0, 0x4146),
    Push(nr0),
    Lsp(nr0),
    Push(4),         # size: 4
    Push(nr0),       # buf: "FAIL"
    Push(2),         # fd: stderr
    Call('func_write'),
    Trm(),
    Label('has_args'),

    # open arg1
    Pop(nr1),        # source filename
    Xor(nr2, nr2),   # open mode - read
    Open(),
    Tst(nr0, nr0),
    Jeq('fail'),     # open failed
    Mov(nr7, nr0),   # save fd in nr7

    # open arg2
    Pop(nr1),        # destination filename
    Add(nr2, 1),     # open mode - write
    Open(),
    Tst(nr0, nr0),
    Jeq('fail'),
    Mov(nr6, nr0),   # save fd in nr6

    # encrypt file
    Push(nr6),       # destination fd
    Push(nr7),       # source fd
    Call('func_encrypt'),

    Mov(nr1, nr6),   # fd
    Cls(),           # close file
    Mov(nr1, nr7),   # fd
    Cls(),           # close file
])

# function func_write
# arg1 - fd
# arg2 - buffer
# arg3 - size
# ret - written len
p.add([
    Label('func_write'),
    Lsp(nr0),
    Push(nr1),
    Push(nr2),
    Push(nr3),
    Ldm(nr1, nr0, 4),
    Ldm(nr2, nr0, 8),
    Ldm(nr3, nr0, 12),
    Wrt(),
    Pop(nr3),
    Pop(nr2),
    Pop(nr1),
    Ret(),
])

# function func_encrypt
# arg1 - source fd
# arg2 - destination fd
p.add([
    Label('func_encrypt'),
    Lsp(nr0),
    Push(nr1),
    Push(nr2),
    Push(nr3),
    Push(nr4),
    Mov(nr4, nr0),

    # alloc local memory
    Lsp(nr3),
    Sub(nr3, 0x100),
    Ssp(nr3),

    Label('func_encrypt_read_loop'),
    Ldm(nr1, nr4, 4),
    Lsp(nr2),
    Mov(nr3, 0x100),
    Read(),
    Tst(nr0, nr0),
    Jeq('func_encrypt_exit'), # EOF
    Mov(nr3, nr0),

    Label('func_encrypt_proces_loop'),

    # try 4 bytes
    Cmp(nr3, 4),
    Jlt('func_encrypt_try_2bytes'),
    Ldm(nr1, nr2, 0),
    Push(nr1),
    Call('func_encrypt_4byte'),
    Pop(nr1),
    Push(nr0),
    Lsp(nr0),
    Push(4),
    Push(nr0),
    Ldm(nr0, nr4, 8),   # destination fd
    Push(nr0),
    Call('func_write'),
    Lsp(nr0),
    Add(nr0, 16),
    Ssp(nr0),
    Add(nr2, 4),
    Sub(nr3, 4),
    Jmp('func_encrypt_proces_loop'),

    # try 2 bytes
    Label('func_encrypt_try_2bytes'),
    Cmp(nr3, 2),
    Jlt('func_encrypt_try_1byte'),
    Ldm(nr1, nr2, 0),
    Push(nr1),
    Call('func_encrypt_2byte'),
    Pop(nr1),
    Push(nr0),
    Lsp(nr0),
    Push(2),
    Push(nr0),
    Ldm(nr0, nr4, 8),   # destination fd
    Push(nr0),
    Call('func_write'),
    Lsp(nr0),
    Add(nr0, 16),
    Ssp(nr0),
    Add(nr2, 2),
    Sub(nr3, 2),
    Jmp('func_encrypt_proces_loop'),

    # try 1 byte
    Label('func_encrypt_try_1byte'),
    Tst(nr3, nr3),
    Jeq('func_encrypt_read_loop'),
    Ldm(nr1, nr2, 0),
    Push(nr1),
    Call('func_encrypt_1byte'),
    Pop(nr1),
    Push(nr0),
    Lsp(nr0),
    Push(1),
    Push(nr0),
    Ldm(nr0, nr4, 8),   # destination fd
    Push(nr0),
    Call('func_write'),
    Lsp(nr0),
    Add(nr0, 16),
    Ssp(nr0),
    Add(nr2, 1),
    Sub(nr3, 1),
    Jmp('func_encrypt_proces_loop'),

    Label('func_encrypt_exit'),
    # dealloc local memory
    Lsp(nr3),
    Add(nr3, 0x100),
    Ssp(nr3),

    Pop(nr4),
    Pop(nr3),
    Pop(nr2),
    Pop(nr1),
    Ret(),
])

# function func_encrypt_4byte
# arg1 - word to encrypt
# return - encrypted word
p.add([
    Label('func_encrypt_4byte'),
    Lsp(nr0),
    Push(nr1),
    Push(nr2),
    Push(nr3),

    Ldm(nr1, nr0, 4),
    Mov(nr0, 0xb73f),
    Shl(nr0, 16),
    Or(nr0, 0xca91),

    # step 1 - bitwise NOT for [31:26]
    Mov(nr2, nr1),
    Shr(nr2, 26),
    Not(nr2),
    Shl(nr2, 26),
    Xor(nr0, nr2),

    # step 2 - rol(5) for [25:16]
    Mov(nr2, nr1),
    Shr(nr2, 16),
    Mov(nr3, nr2),
    And(nr2, 0x1f),
    And(nr3, 0x3e0),
    Shl(nr2, 5),
    Shr(nr3, 5),
    Or(nr2, nr3),
    Shl(nr2, 16),
    Xor(nr0, nr2),
    Mov(nr2, nr0),
    Shr(nr2, 24),
    And(nr2, 0xff),
    Shl(nr2, 16),
    Xor(nr0, nr2),

    # step 3 - swap [15:12] and [11:8]
    Mov(nr2, nr1),
    Shr(nr2, 8),
    Mov(nr3, nr2),
    And(nr2, 0xf),
    And(nr3, 0xf0),
    Shl(nr2, 12),
    Shl(nr3, 4),
    Xor(nr0, nr2),
    Xor(nr0, nr3),
    Mov(nr2, nr0),
    Shr(nr2, 16),
    And(nr2, 0xff),
    Shl(nr2, 8),
    Xor(nr0, nr2),

    # step 4 - xor [7:0] with 0xaa
    Mov(nr2, nr1),
    And(nr2, 0xff),
    Xor(nr2, 0xaa),
    Xor(nr0, nr2),
    Mov(nr2, nr0),
    Shr(nr2, 8),
    And(nr2, 0xff),
    Xor(nr0, nr2),

    Pop(nr3),
    Pop(nr2),
    Pop(nr1),
    Ret(),
])

# function func_encrypt_2byte
# arg1 - halfword to encrypt
# return - encrypted halfword
p.add([
    Label('func_encrypt_2byte'),
    Lsp(nr0),
    Push(nr1),
    Push(nr2),
    Push(nr3),

    Ldm(nr1, nr0, 4),
    Mov(nr0, 0xa6f1),

    # step 1 - rol(1) [15:11]
    Mov(nr2, nr1),
    And(nr2, 0xf800),
    Shr(nr2, 11),
    Mov(nr3, nr2),
    And(nr2, 0x0f),
    And(nr3, 0x10),
    Shl(nr2, 12),
    Shl(nr3, 7),
    Xor(nr0, nr2),
    Xor(nr0, nr3),

    # step 2 - bitwise NOT [10:5]
    Mov(nr2, nr1),
    Not(nr2),
    And(nr2, 0x7e0),
    Xor(nr0, nr2),

    # step 3 - ror(1) [4:0]
    Mov(nr2, nr1),
    And(nr2, 0x1f),
    Mov(nr3, nr2),
    And(nr2, 0x1e),
    And(nr3, 0x01),
    Shr(nr2, 1),
    Shl(nr3, 4),
    Xor(nr0, nr2),
    Xor(nr0, nr3),
    Mov(nr2, nr0),
    Shr(nr2, 8),
    Xor(nr0, nr2),

    Pop(nr3),
    Pop(nr2),
    Pop(nr1),
    Ret(),
])

# function func_encrypt_1byte
# arg1 - byte to encrypt
# return - encrypted byte
p.add([
    Label('func_encrypt_1byte'),
    Lsp(nr0),
    Push(nr1),
    Push(nr2),
    Push(nr3),

    Ldm(nr1, nr0, 4),
    Mov(nr0, 0xb6),

    # step 1 - ror(1) [7:5]
    Mov(nr2, nr1),
    Shr(nr2, 5),
    And(nr2, 7),
    Mov(nr3, nr2),
    And(nr2, 6),
    And(nr3, 1),
    Shl(nr2, 4),
    Shl(nr3, 7),
    Xor(nr0, nr2),
    Xor(nr0, nr3),

    # step 2 - rol(1) [4:2]
    Mov(nr2, nr1),
    Shr(nr2, 2),
    And(nr2, 7),
    Mov(nr3, nr2),
    And(nr2, 3),
    And(nr3, 4),
    Shl(nr2, 3),
    Xor(nr0, nr2),
    Xor(nr0, nr3),

    # step 3 - bitwise NOT [1:0]
    Mov(nr2, nr1),
    Not(nr2),
    And(nr2, 3),
    Xor(nr0, nr2),

    Mov(nr2, nr0),
    And(nr2, 0xf0),
    Shr(nr2, 4),
    Xor(nr0, nr2),

    Pop(nr3),
    Pop(nr2),
    Pop(nr1),
    Ret(),
])
