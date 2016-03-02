# -*- coding: utf-8 -*-

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
    Pop(nr1), # file
    Pop(nr6), # yes
    Pop(nr7), # no
    Open(),
    Mov(nr1, nr0),
    Call('test_jeq'),
    Call('test_jgt')
])

# test for JEQ
# nr1 - file descriptor
# nr6 - yes
# nr7 - no
p.add([
    Label('test_jeq'),
    Mov(nr2, 0x200),
    Mov(nr3, 0x300),
    Add(nr3, nr2),
    Sub(nr2, 0x100),
    Xor(nr3, nr2),
    Cmp(nr3, 0x400),
    Jeq('test_jeq_yes'),
    Mov(nr2, nr7),
    Mov(nr3, 2),
    Wrt(),
    Jmp('test_jeq_fin'),
    Label('test_jeq_yes'),
    Mov(nr2, nr6),
    Mov(nr3, 3),
    Wrt(),
    Label('test_jeq_fin'),
    Ret()
])

# test for JGT
# nr1 - file descriptor
# nr6 - yes
# nr7 - no
p.add([
    Label('test_jgt'),
    Mov(nr2, 0x2001),
    Mov(nr3, 0x2000),
    Cmp(nr2, nr3),
    Jgt('test_jgt_yes'),
    Mov(nr2, nr7),
    Mov(nr3, 2),
    Wrt(),
    Jmp('test_jgt_fin'),
    Label('test_jgt_yes'),
    Mov(nr2, nr6),
    Mov(nr3, 3),
    Wrt(),
    Label('test_jgt_fin'),
    Ret()
])