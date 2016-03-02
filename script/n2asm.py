# -*- coding: utf-8 -*-

import sys
from assembler import *

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print 'Usage: {0} <program.py> [-v]'.format(sys.argv[0])
        print '  program.py: name of n2e program to assemble (exclude .py)'
        print '  Options:'
        print '    -v: if given, assembler will print assembly mnemonics'
        sys.exit(1)

    print 'Assembling program `{0}`'.format(sys.argv[1])
    target = __import__(sys.argv[1])

    verbose = False

    if len(sys.argv) == 3:
        if (sys.argv[2] == '-v'):
            verbose = True

    asm = Assembler()
    asm.assemble(target.p, sys.argv[1] + '.n2e', verbose)
    print 'Assemble done'