/** @file

  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "MicroPython.h"

extern int main (int argc, char **argv);

SBL_SERVICE    sbl_service_tbl;
SBL_SERVICE   *sbl_service;

void  *serivce_map_tbl[] = {
  "DebugPrint",         &sbl_service_tbl.DebugPrint,
  "ConsoleInit",  			&sbl_service_tbl.ConsoleInit,
  "ConsolePoll",        &sbl_service_tbl.ConsolePoll,
  "ConsoleRead",        &sbl_service_tbl.ConsoleRead,
  "ConsoleWrite",       &sbl_service_tbl.ConsoleWrite,
  "MicroSecondDelay",   &sbl_service_tbl.MicroSecondDelay,
  "AllocatePages",      &sbl_service_tbl.AllocatePages,
  "FreePages",          &sbl_service_tbl.FreePages,
  "DrawFrameBuffer",    &sbl_service_tbl.DrawFrameBuffer,
  "SerialPortWrite",    &sbl_service_tbl.SerialPortWrite,
  "LoadFile",           &sbl_service_tbl.LoadFile,
  "BuildConfigData",    &sbl_service_tbl.BuildConfigData,
  "SaveConfigData",     &sbl_service_tbl.SaveConfigData,
  "PlatformReset",      &sbl_service_tbl.PlatformReset,
};

/**
  Payload main entry.

  This function will continue Payload execution with a new memory based stack.

  @param  Param           parameter passed from SwitchStack().

**/
VOID
EFIAPI
SecStartup (
  IN VOID           *HobList,
  IN PLD_MOD_PARAM  *PldModParam
  )
{
  int              idx;
  VOID            *func;
  PLD_MOD_PARAM    pld_mod;

  CopyMem (&pld_mod, PldModParam, sizeof(PLD_MOD_PARAM));
  sbl_service = &sbl_service_tbl;
  for (idx = 0; idx < sizeof(serivce_map_tbl) / sizeof(serivce_map_tbl[0]); idx += 2) {
    func = pld_mod.GetProcAddress (serivce_map_tbl[idx]);
    if (func == NULL) {
      if (idx > 0) {
        assert (func != NULL);
      } else {
        CpuDeadLoop ();
      }
    }
    *(VOID **)serivce_map_tbl[idx + 1] = func;
  }

  DEBUG ((DEBUG_INFO, "Starting MicroPython ...\n"));

  main (1, &pld_mod.CmdLineBuf);

  CpuDeadLoop ();
}
