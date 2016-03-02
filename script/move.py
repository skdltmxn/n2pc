# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Move(Instruction):
    def __init__(self, imm, rd, val):
        Instruction.__init__(self, 3)
        self.imm = imm
        self.rd = rd
        self.val = val

    def raw(self):
        ret = Instruction.raw(self) | (self.imm << 28) | (int(self.rd) << 25)
        if self.imm:
            return ret | (self.val & 0x1ffffff)
        else:
            return ret | (int(self.val) << 22)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        return '{0} {1}, {2}'.format(self.mnemonic(), self.rd, self.val)
        
class Mov(Move):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Move.__init__(self, False, rd, val)
        else:
            Move.__init__(self, True, rd, val)

    def mnemonic(self):
        return 'mov'
