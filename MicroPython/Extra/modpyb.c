/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/mphal.h"
#include "lib/utils/pyexec.h"
#include "extmod/machine_mem.h"
#include "sblservice.h"

STATIC mp_obj_t pyb_console_init(mp_obj_t icol, mp_obj_t irow) {
    mp_int_t         row;
    mp_int_t         col;
    col = mp_obj_get_int(icol);
    row = mp_obj_get_int(irow);
    mp_int_t ret = sbl_service->ConsoleInit (row, col);
    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_console_init_obj, pyb_console_init);

STATIC mp_obj_t pyb_fb_blt(mp_obj_t buf, mp_obj_t icol) {
    mp_int_t         row;
    mp_int_t         col;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    col = mp_obj_get_int(icol);
    row = DivU64x32(bufinfo.len, col * 2);
    mp_uint_t ret = sbl_service->DrawFrameBuffer (row, col, bufinfo.buf);
    return mp_obj_new_bool(ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(pyb_fb_blt_obj, pyb_fb_blt);

STATIC mp_obj_t pyb_uart_write (mp_obj_t buf) {
  mp_buffer_info_t bufinfo;
  mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
  mp_uint_t ret = sbl_service->SerialPortWrite (bufinfo.buf, bufinfo.len);
  return mp_obj_new_int (ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_uart_write_obj, pyb_uart_write);

STATIC mp_obj_t pyb_getch(void) {

  UINT8      key;
  mp_int_t   ret;
  if (sbl_service->ConsolePoll ()) {
    sbl_service->ConsoleRead (&key, 1);
    ret = key;
  } else {
    ret = 0;
  }
  return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(pyb_getch_obj, pyb_getch);


STATIC mp_obj_t pyb_load_file (mp_obj_t path) {
  RETURN_STATUS              status;
  VOID                       *image;
  UINTN                      image_size;

  image       = NULL;
  image_size  = 0;

  const char *file_name = mp_obj_str_get_str(path);
  status = sbl_service->LoadFile (file_name, (VOID **)&image, &image_size);
  mp_obj_t ret = mp_obj_new_bytes(image, image_size);

  if (image != NULL) {
    sbl_service->FreePages (image, EFI_SIZE_TO_PAGES (image_size));
  }

  return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_load_file_obj, pyb_load_file);

STATIC mp_obj_t pyb_build_cfgd (mp_obj_t buf) {
  mp_buffer_info_t bufinfo;
  mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ | MP_BUFFER_WRITE);

  EFI_STATUS  ret;
  ret = sbl_service->BuildConfigData (bufinfo.buf, bufinfo.len);

  return mp_obj_new_bool (ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_build_cfgd_obj, pyb_build_cfgd);

STATIC mp_obj_t pyb_save_cfgd (mp_obj_t buf) {
  mp_buffer_info_t bufinfo;
  mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);

  EFI_STATUS  ret;
  ret = sbl_service->SaveConfigData (bufinfo.buf, bufinfo.len);

  return mp_obj_new_bool (ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_save_cfgd_obj, pyb_save_cfgd);

STATIC mp_obj_t pyb_reboot (mp_obj_t itype) {
  mp_int_t   type;
  type = mp_obj_get_int(itype);
  EFI_STATUS ret;
  ret = sbl_service->PlatformReset (type);
  return mp_obj_new_bool (ret == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_reboot_obj, pyb_reboot);

STATIC const mp_rom_map_elem_t pyb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_pyb) },
    { MP_ROM_QSTR(MP_QSTR_fb_blt),              MP_ROM_PTR(&pyb_fb_blt_obj) },
    { MP_ROM_QSTR(MP_QSTR_console_init),        MP_ROM_PTR(&pyb_console_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_uart_write),          MP_ROM_PTR(&pyb_uart_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_getch),               MP_ROM_PTR(&pyb_getch_obj) },
    { MP_ROM_QSTR(MP_QSTR_load_file),           MP_ROM_PTR(&pyb_load_file_obj) },
    { MP_ROM_QSTR(MP_QSTR_build_cfgd),          MP_ROM_PTR(&pyb_build_cfgd_obj) },
    { MP_ROM_QSTR(MP_QSTR_save_cfgd),           MP_ROM_PTR(&pyb_save_cfgd_obj) },
    { MP_ROM_QSTR(MP_QSTR_reboot),              MP_ROM_PTR(&pyb_reboot_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_module_globals, pyb_module_globals_table);

const mp_obj_module_t pyb_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&pyb_module_globals,
};
