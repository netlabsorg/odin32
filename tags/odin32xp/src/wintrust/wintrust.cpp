/* $Id: wintrust.cpp,v 1.1 2001-12-13 13:01:56 sandervl Exp $
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define ICOM_CINTERFACE 1
#define CINTERFACE

#include <win\windef.h>
#include <win\winerror.h>
#include <win\ole2.h>
#include <misc.h>

/***********************************************************************
 *		WinVerifyTrust (WINTRUST.@)
 */
LONG WINAPI WinVerifyTrust( HWND hwnd, GUID *ActionID,  LPVOID ActionData )
{
    dprintf(("WinVerifyTrust: (hwnd 0x%04x ActionId %p ActionData %p): NOT IMPLEMENTED/STUB (nothing will be verified)",
            hwnd, ActionID,  ActionData));
    return ERROR_SUCCESS;
}
