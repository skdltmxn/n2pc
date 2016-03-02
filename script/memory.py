# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Memory(Instruction):
    def __init__(self, opcode, rd, rs, offset):
        Instruction.__init__(self, 1)
        self.opcode = opcode
        self.rd = rd
        self.rs = rs
        self.offset = offset

    def raw(self):
        return Instruction.raw(self) | (self.opcode << 28) | (int(self.rd) << 25) | (int(self.rs) << 22) | (self.offset & 0x1fffff)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        # ldm
        if self.opcode == 0:
            fmt = '{0} {1}, {2}, {3}'
        else:
            fmt = '{0} {1}, {3}, {2}'

        return fmt.format(self.mnemonic(), self.rd, self.rs, self.offset)

class Ldm(Memory):
    def __init__(self, rd, rs, offset):
        Memory.__init__(self, 0, rd, rs, offset)

    def mnemonic(self):
        return 'ldm'

class Stm(Memory):
    def __init__(self, rd, offset, rs):
        Memory.__init__(self, 1, rd, rs, offset)

    def mnemonic(self):
        return 'stm'
