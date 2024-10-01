from __future__ import with_statement
from pirate.build import *
import os
import re
APPNAME = 'FIExt'
VERSION = '0.1'

pirate_home = os.environ['PIRATE_HOME']
pirate_tools_home = os.environ['PIRATE_TOOLS_HOME']
boost_home = os.environ['BOOST_HOME']
boost_version_match = re.match(r'.*boost_([a-zA-Z0-9._]+)-.*$', boost_home)
boost_version = boost_version_match.group(1)
boost_dbg_sfx = 'vc80-mt-gd-${boost_version}'
boost_opt_sfx = 'vc80-mt-${boost_version}'

with UsableSharedLib('boost') as lib:
    lib.libpaths = '${boost_home}/lib'
    lib.includes = '${boost_home}/include'
    lib.libs_dbg = '''
        boost_program_options-${boost_dbg_sfx}
        boost_date_time-${boost_dbg_sfx}
'''
    lib.libs_opt = '''
        boost_program_options-${boost_opt_sfx}
        boost_date_time-${boost_opt_sfx}
'''

with UsableStaticLib('boost-unittest') as lib:
    lib.libpaths = '${boost_home}/lib'
    lib.includes = '${boost_home}/include'
    lib.libs_dbg = '''
        libboost_unit_test_framework-${boost_dbg_sfx}
'''
    lib.libs_opt = '''
        libboost_unit_test_framework-${boost_opt_sfx}
'''

with UsableSharedLib('pirate') as lib:
    lib.libpaths = '${pirate_home}/lib'
    lib.libs_opt = 'pirate-tr  pirate-utils'
    lib.libs_dbg = 'pirate-tr_d  pirate-utils_d'
    lib.includes = '${pirate_home}/include'
    
with UsableSharedLib('pirate-tools') as lib:
    lib.libpaths = '${pirate_tools_home}/lib'
    lib.libs_opt = 'pirate-inds'
    lib.libs_dbg = 'pirate-inds_d'
    lib.includes = '${pirate_tools_home}/include'


defaults = BuildObj('')
defaults.defines = '''
    _CRT_SECURE_NO_DEPRECATE
    _SCL_SECURE_NO_DEPRECATE
    NOMINMAX
    BOOST_PROGRAM_OPTIONS_DYN_LINK
    BOOST_ALL_NO_LIB=1
'''

defaults.includes = 'src'
defaults.runtime = Runtime.SharedMT

defaults_test = BuildObj('', defaults)
defaults_test.subsys = SubSys.Console
defaults_test.uselibs = 'boost boost-unittest'

with CppSharedLib('strat/FIExt-strat', defaults) as obj:
    obj.defines += ' PIRATE_DYN_LINK'
    obj.uselibs = 'pirate  pirate-tools  boost'
    obj.source_dirs = 'src'
    obj.sources = '''
        src/FIExt-factory.cpp
        src/FIExt-strat.cpp
        src/FIExt-version.cpp
'''

with CppProgram('strat/FIExt-strattest', defaults_test) as obj:
    obj.source_dirs = 'test'
    obj.sources = '''
        test/stratest.cpp
        test/FIExt-test.cpp
'''

with CppProgram('sim/FIExt-sim', defaults) as obj:
    obj.subsys = SubSys.Console
    obj.uselibs = 'pirate  pirate-tools  boost'
    obj.uselibs_local = 'strat/FIExt-strat'
    obj.source_dirs = 'src'
    obj.sources = '''
        src/FIExt-sim.cpp
'''
