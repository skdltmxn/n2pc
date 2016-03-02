# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Branch(Instruction):
    def __init__(self, opcode, r, offset):
        Instruction.__init__(self, 2)
        self.opcode = opcode
        self.r = r
        self.offset_is_label = False

        if isinstance(offset, str):
            self.offset = 0
            self.offset_is_label = True
            self.offset_label = offset
        else:
            self.offset = offset

    def is_offset_label(self):
        return self.offset_is_label == True

    def set_offset(self, offset):
        self.offset = offset

    def get_offset_label(self):
        return self.offset_label

    def raw(self):
        ret = Instruction.raw(self) | (self.opcode << 25) | (self.r << 24)
        if self.r:
            return ret | (int(self.offset) << 21)
        else:
            return ret | (self.offset & 0xffffff)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        return '{0} {1}'.format(self.mnemonic(), self.offset)

class Jmp(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 0, True, val)
        else:
            Branch.__init__(self, 0, False, val)

    def mnemonic(self):
        return 'jmp'

class Jeq(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 1, True, val)
        else:
            Branch.__init__(self, 1, False, val)

    def mnemonic(self):
        return 'jeq'

class Jneq(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 2, True, val)
        else:
            Branch.__init__(self, 2, False, val)

    def mnemonic(self):
        return 'jneq'

class Jgt(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 3, True, val)
        else:
            Branch.__init__(self, 3, False, val)

    def mnemonic(self):
        return 'jgt'

class Jge(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 4, True, val)
        else:
            Branch.__init__(self, 4, False, val)

    def mnemonic(self):
        return 'jge'

class Jlt(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 5, True, val)
        else:
            Branch.__init__(self, 5, False, val)

    def mnemonic(self):
        return 'jlt'

class Jle(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 6, True, val)
        else:
            Branch.__init__(self, 6, False, val)

    def mnemonic(self):
        return 'jle'

class Jhi(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 7, True, val)
        else:
            Branch.__init__(self, 7, False, val)

    def mnemonic(self):
        return 'jhi'

class Jhe(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 8, True, val)
        else:
            Branch.__init__(self, 8, False, val)

    def mnemonic(self):
        return 'jhe'

class Jlo(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 9, True, val)
        else:
            Branch.__init__(self, 9, False, val)

    def mnemonic(self):
        return 'jlo'

class Jloe(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 10, True, val)
        else:
            Branch.__init__(self, 10, False, val)

    def mnemonic(self):
        return 'jloe'

class Call(Branch):
    def __init__(self, val):
        if isinstance(val, Register):
            Branch.__init__(self, 11, True, val)
        else:
            Branch.__init__(self, 11, False, val)

    def mnemonic(self):
        return 'call'

class Ret(Branch):
    def __init__(self):
        Branch.__init__(self, 12, True, 0)

    def mnemonic(self):
        return 'ret'

    def __str__(self):
        return self.mnemonic()