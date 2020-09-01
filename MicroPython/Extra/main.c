/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "py/stackctrl.h"
#include "lib/utils/pyexec.h"
#include "sblservice.h"

uint mp_import_stat (const char *path)
{
  return MP_IMPORT_STAT_NO_EXIST;
}

mp_lexer_t *mp_lexer_new_from_file (const char *filename)
{
  mp_raise_OSError (MP_ENOENT);
}

mp_obj_t mp_builtin_open (size_t n_args, const mp_obj_t *args, mp_map_t *kwargs)
{
  return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_KW (mp_builtin_open_obj, 1, mp_builtin_open);

void do_str (const char *src, mp_parse_input_kind_t input_kind)
{
  nlr_buf_t nlr;
  if (nlr_push (&nlr) == 0) {
    mp_lexer_t *lex = mp_lexer_new_from_str_len (MP_QSTR__lt_stdin_gt_, src, strlen (src), 0);
    qstr source_name = lex->source_name;
    mp_parse_tree_t parse_tree = mp_parse (lex, input_kind);
    mp_obj_t module_fun = mp_compile (&parse_tree, source_name, true);
    mp_call_function_0 (module_fun);
    nlr_pop();
  } else {
    // uncaught exception
    mp_obj_print_exception (&mp_plat_print, (mp_obj_t)nlr.ret_val);
  }
}

char *start_script = \
     "exec(pyb.load_file('!SETP/STPY'))\n";

int main(int argc, char **argv)
{

  mp_stack_ctrl_init();
  mp_stack_set_limit (0x10000);

  uint32_t size = 0x1000000;
  uint8_t *heap = (uint8_t *)sbl_service->AllocatePages (EFI_SIZE_TO_PAGES (size));
  gc_init (heap, heap + size);

  mp_init();

  do_str (start_script,  MP_PARSE_FILE_INPUT);

  pyexec_friendly_repl();

  // clean up
  mp_deinit();

  sbl_service->FreePages (heap, EFI_SIZE_TO_PAGES (size));
}

