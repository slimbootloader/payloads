## @ GenCfgData.py
#
# Copyright (c) 2020 - 2021, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##

import os
import sys
import subprocess

#
# Only support VS toolchain for now
# This tool is only needed to run, if developers has changed,
# added new QSTRs and/or upgraded MicroPython, to reflect the correct QDEF()
# definitions for fixed strings or version strings.
#
sys.dont_write_bytecode = True
sbl_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..'))
sys.path.append (sbl_dir)
from BuildLoader import prep_env, verify_toolchains, run_process

arch     = 'x86'
if (len(sys.argv) > 1) and (sys.argv[1] in ['x64','X64']) :
    arch = 'x64'

curr_dir = os.path.dirname(os.path.abspath(__file__))
os.environ['SBL_SOURCE'] = sbl_dir
if 'WORKSPACE' not in os.environ:
    os.environ['WORKSPACE'] = os.environ['SBL_SOURCE']
prep_env()
verify_toolchains('')
os.chdir(curr_dir)

# create genhdr dir
gen_dir = os.path.realpath('../MicroPython/genhdr')
if not os.path.exists(gen_dir):
    os.makedirs (gen_dir)

# generate files
with open(gen_dir + '/moduledefs.h', 'w') as fp:
    cmd_list = [sys.executable, '../MicroPython/py/makemoduledefs.py', '--vpath', '../MicroPython']

if os.name == 'posix':
    pp_cmd  = 'gcc'
else:
    pp_cmd = os.path.join(os.environ['%s_PREFIX' % os.environ['TOOL_CHAIN']], 'bin', 'host%s' % arch, '%s' % arch, 'cl.exe')

inf_file = '../MicroPython.inf'
cmd_list = [sys.executable, 'genhdr.py', '-a', 'IA32' if arch == 'x86' else 'X64', '-p', '%s' % pp_cmd, inf_file]

subprocess.check_call(cmd_list)

