/* $Id: oleaut32.cpp,v 1.7 2001-05-03 18:18:53 sandervl Exp $ */
/* 
 * OLEAUT32 
 * 
 * Copyright 1999 Sander van Leeuwen (OS/2 Port 990815)
 *
 * Based on Wine code: (ole\compobj.c)
 * 
 *	Copyright 1995	Martin von Loewis
 *	Copyright 1998	Justin Bradford
 *      Copyright 1999  Francis Beaudet
 *  	Copyright 1999  Sylvain St-Germain
 *
 * Project Odin Software License can be found in LICENSE.TXT
 * 
 * TODO: OaBuildVersion has to be changed (as well as GetVersion in kernel32)
 */

#include "oleaut32.h"
#ifdef DEBUG
#define DEBUG_RUNTIME
#endif

#include <debugdefs.h>

//******************************************************************************
//Used to open any private logfiles used in oleaut32
//******************************************************************************
void OpenPrivateLogFiles()
{
#ifdef DEBUG
    OpenPrivateLogFileTypelib();
#endif
}
//******************************************************************************
//Used to close all private logfiles used in oleaut32
//******************************************************************************
void ClosePrivateLogFiles()
{
#ifdef DEBUG
    ClosePrivateLogFileTypelib();
#endif
}


/***********************************************************************
 *           OaBuildVersion           [OLEAUT32.170]
 */
UINT WINAPI OaBuildVersion()
{
#if 1
    dprintf(("OLEAUT32: OaBuildVersion"));
    // Patched DJR 22/9/99 for VB6 runtime.
    return 0x1E0101;
#else
    WINDOWS_VERSION ver = VERSION_GetVersion();

    FIXME("Please report to a.mohr@mailto.de if you get version error messages !\n");
    switch(VersionData[ver].getVersion32)
    {
        case 0x80000a03: /* Win 3.1 */
		return 0x140fd1; /* from Win32s 1.1e */
        case 0xc0000004: /* Win 95 */
	 case 0xc0000a04: /* Win 98: verified same as Win95 */
		return 0x1e10a9; /* some older version: 0x0a0bd3 */
        case 0x04213303: /* NT 3.51 */
		FIXME("NT 3.51 version value unknown !\n");
		return 0x1e10a9; /* value borrowed from Win95 */
        case 0x05650004: /* NT 4.0 */
		return 0x141016;
	default:
		return 0x0;
    }
#endif
}
