/*
 * Win32 common controls implementation
 *
 * Copyright (C) 1999 Achim Hasenmueller
 *
 * Based on the work of the WINE group (www.winehq.com)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _H_COMCTL32
#define _H_COMCTL32

#include "winbase.h"
#include "wingdi.h"
#include "winuser.h"
#include <win/commctrl.h>


/* registers window classes inside this DLL */
void RegisterCOMCTL32WindowClasses(void);



/**************************************************************************
 *  UNDOCUMENTED functions
 */

/* private heap memory functions */

LPVOID WINAPI COMCTL32_Alloc (DWORD);
LPVOID WINAPI COMCTL32_ReAlloc (LPVOID, DWORD);
BOOL WINAPI COMCTL32_Free (LPVOID);
DWORD  WINAPI COMCTL32_GetSize (LPVOID);


#endif /* _H_COMCTL32 */