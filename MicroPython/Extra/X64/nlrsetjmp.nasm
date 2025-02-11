;------------------------------------------------------------------------------
;
; Copyright (c) 2018 - 2020, Intel Corporation. All rights reserved.<BR>
; SPDX-License-Identifier: BSD-2-Clause-Patent
;
; Module Name:
;
;   nlrsetjmp.nasm
;
; Abstract:
;
;------------------------------------------------------------------------------

    DEFAULT REL
    SECTION .text

extern ASM_PFX(nlr_push_tail)

global ASM_PFX(nlr_push)
ASM_PFX(nlr_push):
    pop     rcx                         ; rcx <- return address
    mov     rdx, [rsp]                  ; rdx <- nlr
    mov     [rdx + 0x10], rbx
    mov     [rdx + 0x18], rsi
    mov     [rdx + 0x20], rdi
    mov     [rdx + 0x28], rbp
    mov     [rdx + 0x30], rsp
    mov     [rdx + 0x38], rcx           ; rip value to restore in nlr_jump

    push    rcx                         ; make sure nlr_push_tail return back to nlr_push's caller()
    jmp     ASM_PFX(nlr_push_tail)      ; do the rest in C

global ASM_PFX(asm_nlr_jump)
ASM_PFX(asm_nlr_jump):
    pop     rax                         ; skip return address
    pop     rdx                         ; rdx <- nlr (top)
    pop     rax                         ; rax <- val
    mov     rbx, [rdx + 0x10]
    mov     rsi, [rdx + 0x18]
    mov     rdi, [rdx + 0x20]
    mov     rbp, [rdx + 0x28]
    mov     rsp, [rdx + 0x30]
    jmp     qword[rdx + 0x38]           ; restore "rip"

