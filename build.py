#!/usr/bin/env python

PROJECT = 'FIExt'
VERSION = '$VERSION:1.0:VERSION$'
CODE_MODULES = ['sim', 'strat']

import sys
import os
import os.path as p

PIRATE_HOME = os.environ['PIRATE_HOME']
PIRATE_PY_DIR = p.join(PIRATE_HOME, 'py')
sys.path.insert(0, PIRATE_PY_DIR)

import pirate.build.dist as dist

dist.build(project = PROJECT,
           version = VERSION,
           code_modules = CODE_MODULES,
           copy_dirs = ['py', 'perl', 'shlib', 'etc', 'bin'],
           exec_rexps = [r'/bin/.*\.(?:py|pl|sh|ksh|csh)$'])
