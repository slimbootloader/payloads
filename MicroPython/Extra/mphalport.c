/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/obj.h"
#include "py/stream.h"
#include "py/mpstate.h"
#include "py/mphal.h"
#include "py/lexer.h"
#include "extmod/misc.h"
#include "lib/utils/pyexec.h"
#include "sblservice.h"

uint32_t setjmp(BASE_LIBRARY_JUMP_BUFFER *jump_buf)
{
  return SetJump (jump_buf);
}

void debug_assert (char *file_name, uint32_t line, char *msg)
{
  sbl_service->DebugPrint (DEBUG_ERROR, "ASSERT %a(%d):%a\n", file_name, line, msg);
  CpuDeadLoop ();
}

/**
  Helper method used to print error message.
**/
void stderr_print_strn (void *env, const char *str, size_t len)
{
	sbl_service->ConsoleWrite (str, len);
}

const mp_print_t mp_stderr_print = {NULL, stderr_print_strn};

/**
  Helper method required by MicroPython to handle unrecoverable exception.
**/
void nlr_jump_fail (void *val)
{
  mp_hal_stdout_tx_str ("FATAL: uncaught NLR\n");
	for (;;); // needed to silence compiler warning
}

int usleep (useconds_t us)
{
	sbl_service->MicroSecondDelay (us);
	return 0;
}

void mp_hal_delay_ms(mp_uint_t ms) {
  usleep((ms) * 1000);
}

void mp_hal_delay_us (uint32_t us)
{
	usleep (us);
}

mp_uint_t mp_hal_ticks_us (void)
{
	UINT8 Ratio;
	Ratio = (UINT8)(AsmReadMsr64 (0xCE) >> 8);
	if (Ratio == 0) {
		// This might be QEMU case
		Ratio = 8;
	}
	return DivU64x32 (AsmReadTsc(), Ratio * 100);
}

mp_uint_t mp_hal_ticks_ms (void)
{
	return DivU64x32 (mp_hal_ticks_us(), 1000);
}

mp_uint_t mp_hal_ticks_cpu (void)
{
	return AsmReadTsc();
}

void mp_hal_stdout_tx_strn (const char *str, size_t len)
{
	sbl_service->ConsoleWrite (str, len);
	mp_uos_dupterm_tx_strn (str, len);
}

int mp_hal_stdin_rx_chr (void)
{
	UINT8   ch;

  ch = 0;
	sbl_service->ConsoleRead (&ch, 1);

	return (int)ch;
}


void mp_hal_move_cursor_back(uint pos) {
    if (pos <= 4) {
        // fast path for most common case of 1 step back
        mp_hal_stdout_tx_strn("\b\b\b\b", pos);
    } else {
        char vt100_command[6];
        // snprintf needs space for the terminating null character
        int n = snprintf(&vt100_command[0], sizeof(vt100_command), "\x1b[%u", pos);
        if (n > 0) {
            vt100_command[n] = 'D'; // replace null char
            mp_hal_stdout_tx_strn(vt100_command, n + 1);
        }
    }
}

void mp_hal_erase_line_from_cursor(uint n_chars_to_erase) {
    (void)n_chars_to_erase;
    mp_hal_stdout_tx_strn("\x1b[K", 3);
}


uintptr_t mp_hal_stdio_poll (uintptr_t poll_flags)
{
	uintptr_t ret = 0;
	if ((poll_flags & MP_STREAM_POLL_RD) && sbl_service->ConsolePoll()) {
		ret |= MP_STREAM_POLL_RD;
	}
	return ret;
}
