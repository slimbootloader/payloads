## @ GenCfgData.py
#
# Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
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
sys.path.append (os.path.join(os.path.dirname(__file__), '..', '..', '..'))
from BuildLoader import prep_env, run_process

arch = 'x86'

curr_dir = os.path.dirname(os.path.realpath(__file__))
prep_env()
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

