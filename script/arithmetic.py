# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Arithmetic(Instruction):
    def __init__(self, opcode, imm, rd, val):
        Instruction.__init__(self, 0)
        self.opcode = opcode
        self.rd = rd
        self.imm = imm
        self.val = val

    def raw(self):
        ret = Instruction.raw(self) | (self.opcode << 25) | (self.imm << 24) | (int(self.rd) << 21)
        if self.imm:
            return ret | (self.val & 0x1fffff)
        else:
            return ret | (int(self.val) << 18)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')

    def __str__(self):
        return '{0} {1}, {2}'.format(self.mnemonic(), self.rd, self.val)

class Add(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 0, False, rd, val)
        else:
            Arithmetic.__init__(self, 0, True, rd, val)
    
    def mnemonic(self):
        return 'add'

class Sub(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 1, False, rd, val)
        else:
            Arithmetic.__init__(self, 1, True, rd, val)
        
    def mnemonic(self):
        return 'sub'

class Shl(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 2, False, rd, val)
        else:
            Arithmetic.__init__(self, 2, True, rd, val)
        
    def mnemonic(self):
        return 'shl'

class Shr(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 3, False, rd, val)
        else:
            Arithmetic.__init__(self, 3, True, rd, val)
        
    def mnemonic(self):
        return 'shr'

class Rol(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 4, False, rd, val)
        else:
            Arithmetic.__init__(self, 4, True, rd, val)
        
    def mnemonic(self):
        return 'rol'

class Ror(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 5, False, rd, val)
        else:
            Arithmetic.__init__(self, 5, True, rd, val)
        
    def mnemonic(self):
        return 'ror'

class And(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 6, False, rd, val)
        else:
            Arithmetic.__init__(self, 6, True, rd, val)
        
    def mnemonic(self):
        return 'and'

class Or(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 7, False, rd, val)
        else:
            Arithmetic.__init__(self, 7, True, rd, val)
        
    def mnemonic(self):
        return 'or'

class Xor(Arithmetic):
    def __init__(self, rd, val):
        if isinstance(val, Register):
            Arithmetic.__init__(self, 8, False, rd, val)
        else:
            Arithmetic.__init__(self, 8, True, rd, val)
        
    def mnemonic(self):
        return 'xor'

class Not(Arithmetic):
    def __init__(self, rd):
        Arithmetic.__init__(self, 9, False, rd, 0)
        
    def mnemonic(self):
        return 'not'

    def __str__(self):
        return 'not {0}'.format(self.rd)