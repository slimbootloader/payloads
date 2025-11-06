## @ BuildMicropython.py
#
# Copyright (c) 2025, Intel Corporation. All rights reserved.<BR>
# SPDX-License-Identifier: BSD-2-Clause-Patent
#
##
##
# Import Modules
#
import os
import subprocess
import shutil
import sys
import argparse
import multiprocessing
from importlib.machinery import SourceFileLoader
import types

BuildUtil = types.ModuleType('BuildUtility')

sys.dont_write_bytecode = True

def prep_mpy(arch, sbl_dir):
    mpy_dir = os.path.dirname (os.path.realpath(__file__))
    mpy_pmod_dir = os.path.join(sbl_dir, 'PayloadModPkg')

    if not os.path.exists(mpy_pmod_dir):
        os.makedirs(mpy_pmod_dir)

    # need to build within SBL folder if we're not already there
    if not os.path.samefile(mpy_dir, mpy_pmod_dir):
        shutil.copytree(mpy_dir, mpy_pmod_dir, ignore=shutil.ignore_patterns('.git', 'slimbootloader', 'Build', 'Conf'), dirs_exist_ok=True)

    tool_dir = os.path.join(mpy_pmod_dir, 'MicroPython', 'Tools')
    # create genhdr dir
    gen_dir = os.path.join(mpy_pmod_dir, 'MicroPython', 'MicroPython', 'genhdr')
    if not os.path.exists(gen_dir):
        os.makedirs (gen_dir)

    if os.name == 'posix':
        pp_cmd  = 'gcc'
    else:
        pp_cmd = os.path.join(os.environ['%s_PREFIX' % os.environ['TOOL_CHAIN']], 'bin', 'host%s' % arch, '%s' % arch, 'cl.exe')

    inf_file = os.path.join(mpy_pmod_dir, 'MicroPython', 'MicroPython.inf')
    cmd_list = [sys.executable, os.path.join(tool_dir, 'genhdr.py'), '-a', arch, '-p', '%s' % pp_cmd, inf_file]

    subprocess.run(cmd_list)

def main():
    ap = argparse.ArgumentParser()
    sp = ap.add_subparsers(help='command')

    def cmd_build(args):
        mpy_dir = os.path.dirname (os.path.realpath(__file__))
        os.environ['WORKSPACE'] = mpy_dir
        # Make sure MicroPython submodule is fetched.
        # using '--recursive' here gets a lot of submodules we dont need
        cmd = ['git', 'submodule', 'update', '--init']
        subprocess.run(cmd,cwd=mpy_dir)

        if args.sblpath != '' and os.path.exists(os.path.abspath(args.sblpath)):
            sbl_dir = os.path.abspath(args.sblpath)
        else:
            sbl_dir = os.path.join(mpy_dir, 'slimbootloader')
            if not os.path.exists(os.path.join(sbl_dir,'BootloaderCorePkg','Tools')):
                cmd = ['git', 'clone', 'https://github.com/slimbootloader/slimbootloader.git', sbl_dir]
                subprocess.run(cmd,cwd=mpy_dir)

        sbl_tools_dir = os.path.join(sbl_dir, 'BootloaderCorePkg', 'Tools')
        sys.path.append (sbl_tools_dir)

        os.environ['PACKAGES_PATH'] = sbl_dir

        BuildUtilLoader = SourceFileLoader('BuildUtility', os.path.join(sbl_tools_dir, 'BuildUtility.py'))
        BuildUtil.__file__ =  os.path.join(sbl_tools_dir, 'BuildUtility.py')
        BuildUtilLoader.exec_module(BuildUtil)

        # Verify toolchains first
        BuildUtil.verify_toolchains(args.toolchain)

        prep_mpy(args.arch.upper(), sbl_dir)

        cmd = [
            sys.executable,
            os.path.join(sbl_dir, 'BuildLoader.py'),
            'build_dsc',
            '-p',
            os.path.join(mpy_dir, 'PayloadModPkg.dsc'),
            '-d', 'MICRO_PYTHON',
            '-a', args.arch.lower()
            ]
        if args.release:
            cmd.append('-r')
        subprocess.run(cmd,cwd=mpy_dir)

    def cmd_clean(args):
        shutil.rmtree(os.path.join(os.path.dirname (os.path.realpath(__file__)), 'Build'), ignore_errors=True)

        # if we downloaded the sbl repo, clean that too
        sbl_dir = os.path.join(os.path.dirname (os.path.realpath(__file__)), 'slimbootloader')
        if os.path.exists(sbl_dir):
            if args.del_sbl:
                shutil.rmtree(sbl_dir, ignore_errors=True)
            elif not args.no_clean_sbl:
                cmd = ['git', 'clean', '-xdf']
                subprocess.run(cmd,cwd=sbl_dir)

    buildp = sp.add_parser('build', help='build Micropython PayloadMod')
    buildp.add_argument('-r',  '--release', action='store_true', help='Release build')
    buildp.add_argument('-a',  '--arch', choices=['ia32', 'x64'], help='Specify the ARCH for build. Default is to build IA32 image.', default ='ia32')
    buildp.add_argument('-t', '--toolchain', dest='toolchain', type=str, default='', help='Perferred toolchain name')
    buildp.add_argument('-s', '--sblpath', dest='sblpath', type=str, default='', help='path to SBL source folder')
    buildp.set_defaults(func=cmd_build)

    cleanp = sp.add_parser('clean', help='clean up workspace temp files')
    cleanp.add_argument('-n', dest='no_clean_sbl', action='store_true', help='dont clean sbl source folder')
    cleanp.add_argument('-d', dest='del_sbl', action='store_true', help='Delete SBL source folder instead of cleaning')
    cleanp.set_defaults(func=cmd_clean)

    args = ap.parse_args()
    if len(args.__dict__) <= 1:
        # No arguments or subcommands were given.
        ap.print_help()
        ap.exit()

    args.func(args)

if __name__ == '__main__':
    main()
