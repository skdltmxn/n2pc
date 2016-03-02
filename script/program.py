# -*- coding: utf-8 -*-

from special import Trm

class Program(object):
    def __init__(self, insts):
        self.inst = insts + [Trm()]

    def get_inst(self):
        return self.inst

    def add(self, insts):
        self.inst += insts

    def __str__(self):
        s = ''
        for i in self.inst:
            s += '{0}\n'.format(i)

        return s