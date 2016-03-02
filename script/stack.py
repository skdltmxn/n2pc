# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Stack(Instruction):
    def __init__(self, opcode, imm, val):
        Instruction.__init__(self, 5)
        self.opcode = opcode
        self.imm = imm
        self.val = val

    def raw(self):
        ret = Instruction.raw(self) | (self.opcode << 27) | (self.imm << 26)
        if self.imm:
            return ret | (self.val & 0x3ffffff)
        else:
            return ret | (int(self.val) << 23)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        return '{0} {1}'.format(self.mnemonic(), self.val)
        
class Push(Stack):
    def __init__(self, val):
        if isinstance(val, Register):
            Stack.__init__(self, 0, False, val)
        else:
            Stack.__init__(self, 0, True, val)

    def mnemonic(self):
        return 'push'

class Pop(Stack):
    def __init__(self, rd):
        Stack.__init__(self, 1, False, rd)

    def mnemonic(self):
        return 'pop'

class Lsp(Stack):
    def __init__(self, rd):
        Stack.__init__(self, 2, False, rd)

    def mnemonic(self):
        return 'lsp'

class Ssp(Stack):
    def __init__(self, rs):
        Stack.__init__(self, 3, False, rs)

    def mnemonic(self):
        return 'ssp'
