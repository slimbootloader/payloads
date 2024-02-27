Slim Bootloader Payloads
========================

**Welcome to the Slim Bootloader payloads project!**

This repo contains several additional payloads porting for Slim Bootloader.


Steps to Build a Payload Module
--------------------------------
- Clone latest SBL tree and prepare the build environment following this [link](https://slimbootloader.github.io/getting-started/build-host-setup.html). \
  *git clone https://github.com/slimbootloader/slimbootloader.git SlimBoot*

- Clone payloads repo into SBL tree \
  *cd SlimBoot* \
  *git clone https://github.com/slimbootloader/payloads.git PayloadModPkg* \
  *cd PayloadModPkg* \
  *git submodule update --init* \
  *cd ..*

- Run preprocessing if required \
  For example, for MicroPython it is required to generate build header files \
  &nbsp;&nbsp; *python PayloadModPkg\MicroPython\Tools\prep.py*

- Build payload module binary \
  For example, to build MicroPython payload module, run \
  &nbsp;&nbsp; *python BuildLoader.py build_dsc -p PayloadModPkg\PayloadModPkg.dsc -d MICRO_PYTHON*

- Copy generated binary to payload binary directory
  For example, to copy MicroPython binary, run \
  &nbsp;&nbsp; *mkdir PayloadPkg\PayloadBins* \
  &nbsp;&nbsp; *copy  Build\PayloadModPkg\DEBUG_VS2017\IA32\MicroPython.efi PayloadPkg\PayloadBins /y*


Steps to Integrate MicroPython
-------------------------------
- Ensure MicroPython.efi is located in PayloadPkg\PayloadBins directory \
  If not, please build MicroPython payload module as mentioned above.

- Enable QEMU SBL Setup feature \
  Set self.ENABLE_SBL_SETUP value to 1 in file:\
  Platform\QemuBoardPkg\BoardConfig.py

- Build SBL as normal \
  *python BuildLoader.py build qemu -k*

- Boot QEMU and enable MicroPython setup payload module launching \
  *qemu-system-x86_64 -m 256M -cpu max -machine q35 -drive if=pflash,format=raw,file=Outputs\qemu\SlimBootloader.bin -serial telnet:127.0.0.1:8888,server -boot order=a*

- Start a telnet console using Putty telnet 127.0.0.1 port 8888 to connect to QEMU \
  Make sure the console screen is set to 100 x 30 with Linux keypad for terminal keyboard.

