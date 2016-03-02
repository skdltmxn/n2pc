# -*- coding: utf-8 -*-

from instruction import *
from register import *

class Label(Instruction):
    def __init__(self, label):
        self.label = label

    def raw(self):
        raise FakeExceptionLabel

    def get_label(self):
        return self.label
