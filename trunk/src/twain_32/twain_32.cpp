/* $Id: twain_32.cpp,v 1.1 2000-01-04 22:19:01 sandervl Exp $ */

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

#include <twain.h>

#include "misc.h"


TW_UINT16 (APIENTRY *TWAINOS2_DSM_Entry)( pTW_IDENTITY, pTW_IDENTITY,
	   TW_UINT32, TW_UINT16, TW_UINT16, TW_MEMREF) = 0;


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
    static HINSTANCE hInstance = 0;
    hInstance = LoadLibraryA("TWAINOS2.DLL");
    if(hInstance) 
    {   
        *(VOID **)&TWAINOS2_DSM_Entry=(void*)GetProcAddress(hInstance, (LPCSTR)"DSM_Entry");
    }
    WriteLog("TWAIN_32: DSM_Entry called with parameters:");
    WriteLog("TWAIN_32: pOrigin.Id: %08xh ", pOrigin->Id);
//    WriteLog("TWAIN_32: pOrigin.Version: %08xh ", pOrigin->Version);
//    WriteLog("TWAIN_32: pOrigin.ProtocolMajor: %08xh ", pOrigin->ProtocolMajor);
//    WriteLog("TWAIN_32: pOrigin.ProtocolMinor: %08xh ", pOrigin->ProtocolMinor);
    WriteLog("TWAIN_32: pOrigin.SupportedGroups: %08xh ", pOrigin->SupportedGroups);
    WriteLog("TWAIN_32: pOrigin.Manufacturer: %s ", pOrigin->Manufacturer);
    WriteLog("TWAIN_32: pOrigin.ProductFamily: %s ", pOrigin->ProductFamily);
    WriteLog("TWAIN_32: pOrigin.Productname: %s ", pOrigin->ProductName);
    WriteLog("TWAIN_32: pDest: %08xh ", pDest);
    WriteLog("TWAIN_32: Data Group: %04xh ", DG);
    WriteLog("TWAIN_32: Data Attribute Type: %04xh ", DAT);
    WriteLog("TWAIN_32: Message: %04xh ", MSG);
    WriteLog("TWAIN_32: pData: %08xh ", pData);
    returnval = TWAINOS2_DSM_Entry( pOrigin, pDest, DG, DAT, MSG, pData);	
    WriteLog("TWAIN_32: DSM_Entry returned: %04xh \n\n", returnval);
    if (returnval == 3)
    {
       MessageBoxA(0, "No DataSource found.", "ODIN TWAIN_32", MB_OK);
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

