# -*- coding: utf-8 -*-

class Register(object):
    def __init__(self, n):
        self.n = n

    def __str__(self):
        return 'nr{0}'.format(self.n)

    def __int__(self):
        return self.n

nr0 = Register(0)
nr1 = Register(1)
nr2 = Register(2)
nr3 = Register(3)
nr4 = Register(4)
nr5 = Register(5)
nr6 = Register(6)
nr7 = Register(7)
