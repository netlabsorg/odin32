/* $Id: twain_32.cpp,v 1.3 2000-01-10 23:27:58 sandervl Exp $ */

/*
 * TWAIN_32 implementation
 *
 * Copyright 2000 Jens Wiessner
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <memory.h>
#include <misc.h>
#include <twain.h>

extern TW_UINT16 (APIENTRY *TWAINOS2_DSM_Entry)( pTW_IDENTITY, pTW_IDENTITY,
	   TW_UINT32, TW_UINT16, TW_UINT16, TW_MEMREF);


//******************************************************************************
//******************************************************************************
TW_UINT16 FAR PASCAL DSM_Entry( pTW_IDENTITY pOrigin,
                                pTW_IDENTITY pDest,
                                TW_UINT32    DG,
                                TW_UINT16    DAT,
                                TW_UINT16    MSG,
                                TW_MEMREF    pData)
{
    int returnval;
    dprintf(("TWAIN_32: DSM_Entry called with parameters:"));
    dprintf(("TWAIN_32: pOrigin.Id: %08xh ", pOrigin->Id));
//    dprintf(("TWAIN_32: pOrigin.Version: %08xh ", pOrigin->Version));
    dprintf(("TWAIN_32: pOrigin.ProtocolMajor: %08xh ", pOrigin->ProtocolMajor));
    dprintf(("TWAIN_32: pOrigin.ProtocolMinor: %08xh ", pOrigin->ProtocolMinor));
    dprintf(("TWAIN_32: pOrigin.SupportedGroups: %08xh ", pOrigin->SupportedGroups));
    dprintf(("TWAIN_32: pOrigin.Manufacturer: %s ", pOrigin->Manufacturer));
    dprintf(("TWAIN_32: pOrigin.ProductFamily: %s ", pOrigin->ProductFamily));
    dprintf(("TWAIN_32: pOrigin.Productname: %s ", pOrigin->ProductName));
    dprintf(("TWAIN_32: pDest: %08xh ", pDest));
    dprintf(("TWAIN_32: Data Group: %04xh ", DG));
    dprintf(("TWAIN_32: Data Attribute Type: %04xh ", DAT));
    dprintf(("TWAIN_32: Message: %04xh ", MSG));
    dprintf(("TWAIN_32: pData: %08xh ", pData));
    returnval = TWAINOS2_DSM_Entry( pOrigin, pDest, DG, DAT, MSG, pData);
    dprintf(("TWAIN_32: DSM_Entry returned: %04xh \n\n", returnval));
    if (returnval == 3)
    {
       MessageBoxA(0, "No DataSource found.", "ODIN TWAIN_32", MB_OK);
       dprintf(("TWAIN_32: No DataSource found. If you use CFM Twain you should just select Aquire from your Twain enabled app."));
    }
    return returnval;
}

INT CDECL ODIN_ChooseDlgProc(DWORD ret)
{
  dprintf(("TWAIN_32: ChooseDlgProc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

INT CDECL ODIN_AboutDlgProc(DWORD ret)
{
  dprintf(("TWAIN_32: AboutDlgProc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

INT CDECL ODIN_WGDlgProc(DWORD ret)
{
  dprintf(("TWAIN_32: WGDlgProc not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

INT CDECL ODIN_InfoHook(DWORD ret)
{
  dprintf(("TWAIN_32: InfoHook not implemented.\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

