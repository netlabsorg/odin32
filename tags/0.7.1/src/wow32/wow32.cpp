/* $Id: wow32.cpp,v 1.1 2001-05-28 18:13:47 phaller Exp $ */

/*
 * Win32 WOW32 APIs for OS/2
 *
 * Copyright 2001      Patrick Haller <patrick.haller@innotek.de>
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */


#include <os2win.h>
#include <odinwrap.h>
#include <misc.h>
#include <unicode.h>
#include <heapstring.h>
#include <wow32.h>

ODINDEBUGCHANNEL(WOW32)


/*****************************************************************************
 * Name      : DWORD WOWHandle16
 * Purpose   : 
 * Parameters: 
 * Variables :
 * Result    : 
 * Remark    : This function resides in KERNEL32 in Win98. However,
 *             for ODIN, we can place it here very well.
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 2001-05-28]
 *****************************************************************************/


ODINFUNCTION2(DWORD, WOWHandle16,
              DWORD, arg1,
              DWORD, arg2)
{
    dprintf(("WOW32: WowHandle16 not implemented.\n"));

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);

    return 0;
}
