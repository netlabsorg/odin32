/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Win32 OLE stubs for OS/2
 *
 * 1998/06/12
 *
 * Copyright 1998 Sander van Leeuwen
 */

#include "ole32.h"

// ----------------------------------------------------------------------
// CoBuildVersion()
// ----------------------------------------------------------------------
DWORD WIN32API CoBuildVersion()
{
    dprintf(("OLE32.CoBuildVersion\n"));
    return (rmm<<16)+rup;
}

