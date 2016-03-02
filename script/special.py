# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Special(Instruction):
    def __init__(self, opcode):
        Instruction.__init__(self, 6)
        self.opcode = opcode

    def raw(self):
        return Instruction.raw(self) | (self.opcode << 26)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        return self.mnemonic()
        
class Open(Special):
    def __init__(self):
        Special.__init__(self, 0)

    def mnemonic(self):
        return 'open'

class Read(Special):
    def __init__(self):
        Special.__init__(self, 1)

    def mnemonic(self):
        return 'read'

class Wrt(Special):
    def __init__(self):
        Special.__init__(self, 2)

    def mnemonic(self):
        return 'wrt'

class Cls(Special):
    def __init__(self):
        Special.__init__(self, 3)

    def mnemonic(self):
        return 'cls'

class Trm(Special):
    def __init__(self):
        Special.__init__(self, 4)

    def mnemonic(self):
        return 'trm'
