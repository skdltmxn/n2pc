# -*- coding: utf-8 -*-

class Instruction(object):
    def __init__(self, ty):
        self.type = ty & 7

    def raw(self):
        return (self.type << 29)

    def mnemonic(self):
        raise Exception('Invalid mnemonic')