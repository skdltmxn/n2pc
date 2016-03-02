# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Test(Instruction):
    def __init__(self, opcode, imm, ra, val):
        Instruction.__init__(self, 4)
        self.opcode = opcode
        self.imm = imm
        self.ra = ra
        self.val = val

    def raw(self):
        ret = Instruction.raw(self) | (self.opcode << 28) | (self.imm << 27) | (int(self.ra) << 24)
        if self.imm:
            return ret | (self.val & 0x1fffff)
        else:
            return ret | (int(self.val) << 21)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        return '{0} {1}, {2}'.format(self.mnemonic(), self.ra, self.val)
        
class Tst(Test):
    def __init__(self, ra, val):
        if isinstance(val, Register):
            Test.__init__(self, 0, False, ra, val)
        else:
            Test.__init__(self, 0, True, ra, val)

    def mnemonic(self):
        return 'tst'

class Cmp(Test):
    def __init__(self, ra, val):
        if isinstance(val, Register):
            Test.__init__(self, 1, False, ra, val)
        else:
            Test.__init__(self, 1, True, ra, val)

    def mnemonic(self):
        return 'cmp'
