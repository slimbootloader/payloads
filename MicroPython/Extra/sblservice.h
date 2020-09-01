/** @file

  Copyright (c) 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __SBL_SERVICE_H__
#define __SBL_SERVICE_H__

#include <PiPei.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#undef   DEBUG
#define  DEBUG(Expression)   sbl_service->DebugPrint Expression

typedef BOOLEAN    (EFIAPI *CONSOLE_POLL) (VOID);
typedef UINTN      (EFIAPI *CONSOLE_READ) (OUT UINT8 *Buffer,  IN UINTN  NumberOfBytes);
typedef UINTN      (EFIAPI *CONSOLE_WRITE) (IN UINT8 *Buffer,  IN UINTN  NumberOfBytes);
typedef EFI_STATUS (EFIAPI *CONSOLE_INIT) (IN UINT32 Row, IN UINT32 Col);
typedef UINTN      (EFIAPI *MICRO_SECOND_DELAY) (IN UINTN MicroSeconds);
typedef VOID *     (EFIAPI *ALLOCATE_PAGES) (IN UINTN  Pages);
typedef VOID       (EFIAPI *FREE_PAGES) (IN VOID *Buffer,  IN UINTN  Pages);
typedef EFI_STATUS (EFIAPI *LOAD_FILE) (IN CHAR8 *Path, IN OUT VOID **Buffer,  IN OUT UINT32 *Length);
typedef UINTN      (EFIAPI *DRAW_FRAME_BUFFER) (IN UINT32 Col, IN UINT32 Row,  IN UINT8 *Buffer);
typedef UINTN      (EFIAPI *UART_WRITE) (IN UINT8 *Buffer,  IN UINTN  NumberOfBytes);
typedef EFI_STATUS (EFIAPI *BUILD_CFG_DATA) (IN UINT8 *Buffer, IN  UINT32 Length);
typedef EFI_STATUS (EFIAPI *SAVE_CFG_DATA) (IN UINT8 *Buffer, IN  UINT32 Length);
typedef VOID       (EFIAPI *DEBUG_ASSERT) (IN CONST CHAR8  *FileName, IN UINTN LineNumber, IN CONST CHAR8  *Description);
typedef VOID       (EFIAPI *DEBUG_PRINT) (IN UINTN ErrorLevel, IN CONST CHAR8  *Format, ...);
typedef EFI_STATUS (EFIAPI *PLATFORM_RESET) (IN UINT32 Type);

typedef struct {
  DEBUG_PRINT               DebugPrint;
  CONSOLE_INIT              ConsoleInit;
  CONSOLE_POLL              ConsolePoll;
  CONSOLE_READ              ConsoleRead;
  CONSOLE_WRITE             ConsoleWrite;
  MICRO_SECOND_DELAY        MicroSecondDelay;
  ALLOCATE_PAGES            AllocatePages;
  FREE_PAGES                FreePages;
  DRAW_FRAME_BUFFER         DrawFrameBuffer;
  UART_WRITE                SerialPortWrite;
  LOAD_FILE                 LoadFile;
  BUILD_CFG_DATA            BuildConfigData;
  SAVE_CFG_DATA             SaveConfigData;
  PLATFORM_RESET            PlatformReset;
} SBL_SERVICE;

extern SBL_SERVICE  *sbl_service;

#endif
