# -*- coding: utf-8 -*-

import struct
from program import *
from label import Label
from branch import Branch

class Assembler(object):
    def __init__(self):
        self.code = ''
        self.stage = 1
        self.labels = {}

    # process labels fist
    def stage1(self, inst):
        for i in inst:
            if isinstance(i, Label):
                label = i.get_label()

                if self.labels.has_key(label):
                    raise Exception('Label {0} already exists'.format(label))
                else:
                    self.labels[label] = inst.index(i)

                inst.remove(i)

        return inst

    # assemble
    def stage2(self, inst, verbose):
        for i in inst:
            if isinstance(i, Branch):
                if i.is_offset_label():
                    label = i.get_offset_label()
                    if self.labels.has_key(label) == False:
                        raise Exception('Undefiend label {0} found'.format(label))

                    i.set_offset(self.labels[label] - inst.index(i))

            if verbose:
                print str(i)

            self.code += struct.pack('<I', i.raw())

    def assemble(self, program, output, verbose = False):
        inst = program.get_inst()
        
        inst = self.stage1(inst)
        self.stage2(inst, verbose)

        with open(output, 'wb') as f:
            f.write('\x02')
            f.write(struct.pack('<I', len(self.code)))
            f.write(self.code)