/* $Id: FastInfoBlocks.cpp,v 1.1 2003-03-06 10:22:25 sandervl Exp $
 *
 *
 *
 * Copyright (c) 2003 knut st. osmundsen <bird@anduin.net>
 *
 * Project Odin Software License can be found in LICENSE.TXT.
 *
 */

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <odin.h>
#include <os2wrap.h>
#include <dbglog.h>
#include <FastInfoBlocks.h>


/*
 * We must make sure that the pointers are initiated before any other
 * C++ objects. (VMutex in particualar)
 * Use the C++ static object trick.
 */
#pragma priority(-2147482623)           /* we will init first! */

class fibInitClass
{
public:
    fibInitClass()
    {
        fibInit();
    }
};

fibInitClass    obj;


/**
 * Logs the content of all the structures.
 */
void    fibDumpAll(void)
{
    dprintf(("KERNEL32: _gpfibPIB=0x%8x\n", _gpfibPIB));
    if (_gpfibPIB)
    {
        dprintf(("KERNEL32:  pib_ulpid    = %08x\n", _gpfibPIB->pib_ulpid    ));
        dprintf(("KERNEL32:  pib_ulppid   = %08x\n", _gpfibPIB->pib_ulppid   ));
        dprintf(("KERNEL32:  pib_hmte     = %08x\n", _gpfibPIB->pib_hmte     ));
        dprintf(("KERNEL32:  pib_pchcmd   = %08x\n", _gpfibPIB->pib_pchcmd   ));
        dprintf(("KERNEL32:  pib_pchenv   = %08x\n", _gpfibPIB->pib_pchenv   ));
        dprintf(("KERNEL32:  pib_flstatus = %08x\n", _gpfibPIB->pib_flstatus ));
        dprintf(("KERNEL32:  pib_ultype   = %08x\n", _gpfibPIB->pib_ultype   ));
    }
    dprintf(("KERNEL32: _gpfibLIS = %08x\n", _gpfibLIS));
    if (_gpfibLIS)
    {
        dprintf(("KERNEL32:  pidCurrent     = %08x\n", (unsigned)_gpfibLIS->pidCurrent    ));
        dprintf(("KERNEL32:  pidParent      = %08x\n", (unsigned)_gpfibLIS->pidParent     ));
        dprintf(("KERNEL32:  prtyCurrent    = %08x\n", (unsigned)_gpfibLIS->prtyCurrent   ));
        dprintf(("KERNEL32:  tidCurrent     = %08x\n", (unsigned)_gpfibLIS->tidCurrent    ));
        dprintf(("KERNEL32:  sgCurrent      = %08x\n", (unsigned)_gpfibLIS->sgCurrent     ));
        dprintf(("KERNEL32:  rfProcStatus   = %08x\n", (unsigned)_gpfibLIS->rfProcStatus  ));
        dprintf(("KERNEL32:  LIS_fillbyte1  = %08x\n", (unsigned)_gpfibLIS->LIS_fillbyte1 ));
        dprintf(("KERNEL32:  fFoureground   = %08x\n", (unsigned)_gpfibLIS->fFoureground  ));
        dprintf(("KERNEL32:  typeProcess    = %08x\n", (unsigned)_gpfibLIS->typeProcess   ));
        dprintf(("KERNEL32:  LIS_fillbyte2  = %08x\n", (unsigned)_gpfibLIS->LIS_fillbyte2 ));
        dprintf(("KERNEL32:  selEnv         = %08x\n", (unsigned)_gpfibLIS->selEnv        ));
        dprintf(("KERNEL32:  offCmdLine     = %08x\n", (unsigned)_gpfibLIS->offCmdLine    ));
        dprintf(("KERNEL32:  cbDataSegment  = %08x\n", (unsigned)_gpfibLIS->cbDataSegment ));
        dprintf(("KERNEL32:  cbStack        = %08x\n", (unsigned)_gpfibLIS->cbStack       ));
        dprintf(("KERNEL32:  cbHeap         = %08x\n", (unsigned)_gpfibLIS->cbHeap        ));
        dprintf(("KERNEL32:  hmod           = %08x\n", (unsigned)_gpfibLIS->hmod          ));
        dprintf(("KERNEL32:  selDS          = %08x\n", (unsigned)_gpfibLIS->selDS         ));
        dprintf(("KERNEL32:  LIS_PackSel    = %08x\n", (unsigned)_gpfibLIS->LIS_PackSel   ));
        dprintf(("KERNEL32:  LIS_PackShrSel = %08x\n", (unsigned)_gpfibLIS->LIS_PackShrSel));
        dprintf(("KERNEL32:  LIS_PackPckSel = %08x\n", (unsigned)_gpfibLIS->LIS_PackPckSel));
    }
    dprintf(("KERNEL32: _gpfibGIS=%08x\n", _gpfibGIS));
    if (_gpfibGIS)
    {
        dprintf(("KERNEL32:  SIS_BigTime        = %08x\n", (unsigned)_gpfibGIS->SIS_BigTime       ));
        dprintf(("KERNEL32:  SIS_MsCount        = %08x\n", (unsigned)_gpfibGIS->SIS_MsCount       ));
        dprintf(("KERNEL32:  SIS_HrsTime        = %08x\n", (unsigned)_gpfibGIS->SIS_HrsTime       ));
        dprintf(("KERNEL32:  SIS_MinTime        = %08x\n", (unsigned)_gpfibGIS->SIS_MinTime       ));
        dprintf(("KERNEL32:  SIS_SecTime        = %08x\n", (unsigned)_gpfibGIS->SIS_SecTime       ));
        dprintf(("KERNEL32:  SIS_HunTime        = %08x\n", (unsigned)_gpfibGIS->SIS_HunTime       ));
        dprintf(("KERNEL32:  SIS_TimeZone       = %08x\n", (unsigned)_gpfibGIS->SIS_TimeZone      ));
        dprintf(("KERNEL32:  SIS_ClkIntrvl      = %08x\n", (unsigned)_gpfibGIS->SIS_ClkIntrvl     ));
        dprintf(("KERNEL32:  SIS_DayDate        = %08x\n", (unsigned)_gpfibGIS->SIS_DayDate       ));
        dprintf(("KERNEL32:  SIS_MonDate        = %08x\n", (unsigned)_gpfibGIS->SIS_MonDate       ));
        dprintf(("KERNEL32:  SIS_YrsDate        = %08x\n", (unsigned)_gpfibGIS->SIS_YrsDate       ));
        dprintf(("KERNEL32:  SIS_DOWDate        = %08x\n", (unsigned)_gpfibGIS->SIS_DOWDate       ));
        dprintf(("KERNEL32:  SIS_VerMajor       = %08x\n", (unsigned)_gpfibGIS->SIS_VerMajor      ));
        dprintf(("KERNEL32:  SIS_VerMinor       = %08x\n", (unsigned)_gpfibGIS->SIS_VerMinor      ));
        dprintf(("KERNEL32:  SIS_RevLettr       = %08x\n", (unsigned)_gpfibGIS->SIS_RevLettr      ));
        dprintf(("KERNEL32:  SIS_CurScrnGrp     = %08x\n", (unsigned)_gpfibGIS->SIS_CurScrnGrp    ));
        dprintf(("KERNEL32:  SIS_MaxScrnGrp     = %08x\n", (unsigned)_gpfibGIS->SIS_MaxScrnGrp    ));
        dprintf(("KERNEL32:  SIS_HugeShfCnt     = %08x\n", (unsigned)_gpfibGIS->SIS_HugeShfCnt    ));
        dprintf(("KERNEL32:  SIS_ProtMdOnly     = %08x\n", (unsigned)_gpfibGIS->SIS_ProtMdOnly    ));
        dprintf(("KERNEL32:  SIS_FgndPID        = %08x\n", (unsigned)_gpfibGIS->SIS_FgndPID       ));
        dprintf(("KERNEL32:  SIS_Dynamic        = %08x\n", (unsigned)_gpfibGIS->SIS_Dynamic       ));
        dprintf(("KERNEL32:  SIS_MaxWait        = %08x\n", (unsigned)_gpfibGIS->SIS_MaxWait       ));
        dprintf(("KERNEL32:  SIS_MinSlice       = %08x\n", (unsigned)_gpfibGIS->SIS_MinSlice      ));
        dprintf(("KERNEL32:  SIS_MaxSlice       = %08x\n", (unsigned)_gpfibGIS->SIS_MaxSlice      ));
        dprintf(("KERNEL32:  SIS_BootDrv        = %08x\n", (unsigned)_gpfibGIS->SIS_BootDrv       ));
        dprintf(("KERNEL32:  SIS_MaxVioWinSG    = %08x\n", (unsigned)_gpfibGIS->SIS_MaxVioWinSG   ));
        dprintf(("KERNEL32:  SIS_MaxPresMgrSG   = %08x\n", (unsigned)_gpfibGIS->SIS_MaxPresMgrSG  ));
        dprintf(("KERNEL32:  SIS_SysLog         = %08x\n", (unsigned)_gpfibGIS->SIS_SysLog        ));
        dprintf(("KERNEL32:  SIS_MMIOBase       = %08x\n", (unsigned)_gpfibGIS->SIS_MMIOBase      ));
        dprintf(("KERNEL32:  SIS_MMIOAddr       = %08x\n", (unsigned)_gpfibGIS->SIS_MMIOAddr      ));
        dprintf(("KERNEL32:  SIS_MaxVDMs        = %08x\n", (unsigned)_gpfibGIS->SIS_MaxVDMs       ));
        dprintf(("KERNEL32:  SIS_Reserved       = %08x\n", (unsigned)_gpfibGIS->SIS_Reserved      ));
    }
}
