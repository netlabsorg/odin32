/* $Id: mem.cpp,v 1.1 2000-05-15 02:43:16 phaller Exp $ */

/*
 * Win32 Lightweight SHELL32 for OS/2
 *
 * Copyright 1997 Marcus Meissner
 * Copyright 1999 Patrick Haller (haller@zebra.fh-weingarten.de)
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Path Functions
 *
 * Many of this functions are in SHLWAPI.DLL also
 *
 * Corel WINE 20000324 level (without CRTDLL_* calls)
 */

/*****************************************************************************
 * Remark                                                                    *
 *****************************************************************************

 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <string.h>
#include <ctype.h>
#include <wctype.h>
#include <wcstr.h>
#define HAVE_WCTYPE_H

#include "debugtools.h"

#include <winreg.h>

#include <heapstring.h>
#include <misc.h>
#include <win\shell.h>
#include <win\winerror.h>
#include <winversion.h>
#include <winuser.h>


#define ICOM_CINTERFACE 1
#define CINTERFACE 1

#include "winerror.h"
#include "winnls.h"
#include "winversion.h"
#include "heap.h"

#include "shellapi.h"
#include "shlobj.h"
#include "wine/undocshell.h"

#include "shlwapi.h"

/*****************************************************************************
 * Local Variables                                                           *
 *****************************************************************************/

ODINDEBUGCHANNEL(SHLWAPI-MEM)


/*************************************************************************
 * SHFree                                       [SHELL32.195]
 *
 * NOTES
 *     free_ptr() - frees memory using IMalloc
 *     exported by ordinal
 */
#define MEM_DEBUG 0
DWORD WINAPI SHFree(LPVOID x)
{
#if MEM_DEBUG
        WORD len = *(LPWORD)(x-2);

        if ( *(LPWORD)(x+len) != 0x7384)
          ERR("MAGIC2!\n");

        if ( (*(LPWORD)(x-4)) != 0x8271)
          ERR("MAGIC1!\n");
        else
          memset(x-4, 0xde, len+6);

        TRACE("%p len=%u\n",x, len);

        x -= 4;
#else
        TRACE("%p\n",x);
#endif
        return HeapFree(GetProcessHeap(), 0, x);
}

/*************************************************************************
 * SHAlloc                                      [SHELL32.196]
 *
 * NOTES
 *     void *task_alloc(DWORD len), uses SHMalloc allocator
 *     exported by ordinal
 */
LPVOID WINAPI SHAlloc(DWORD len)
{
        LPBYTE ret;

#if MEM_DEBUG
        ret = (LPBYTE) HeapAlloc(GetProcessHeap(),0,len+6);
#else
        ret = (LPBYTE) HeapAlloc(GetProcessHeap(),0,len);
#endif

#if MEM_DEBUG
        *(LPWORD)(ret) = 0x8271;
        *(LPWORD)(ret+2) = (WORD)len;
        *(LPWORD)(ret+4+len) = 0x7384;
        ret += 4;
        memset(ret, 0xdf, len);
#endif
        TRACE("%lu bytes at %p\n",len, ret);
        return (LPVOID)ret;
}

/*************************************************************************
 * SHAllocShared                                [SHELL32.520]
 *
 * NOTES
 *  parameter1 is return value from HeapAlloc
 *  parameter2 is equal to the size allocated with HeapAlloc
 *  parameter3 is return value from GetCurrentProcessId
 *
 *  the return value is posted as lParam with 0x402 (WM_USER+2) to somewhere
 *  WM_USER+2 could be the undocumented CWM_SETPATH
 *  the allocated memory contains a pidl
 */
HGLOBAL WINAPI SHAllocShared(LPVOID psrc, DWORD size, DWORD procID)
{       HGLOBAL hmem;
        LPVOID pmem;

        TRACE("ptr=%p size=0x%04lx procID=0x%04lx\n",psrc,size,procID);
        hmem = GlobalAlloc(GMEM_FIXED, size);
        if (!hmem)
          return 0;

        pmem =  GlobalLock (hmem);

        if (! pmem)
          return 0;

        memcpy (pmem, psrc, size);
        GlobalUnlock(hmem);
        return hmem;
}
/*************************************************************************
 * SHLockShared                                 [SHELL32.521]
 *
 * NOTES
 *  parameter1 is return value from SHAllocShared
 *  parameter2 is return value from GetCurrentProcessId
 *  the receiver of (WM_USER+2) trys to lock the HANDLE (?)
 *  the returnvalue seems to be a memoryadress
 */
LPVOID WINAPI SHLockShared(HANDLE hmem, DWORD procID)
{       TRACE("handle=0x%04x procID=0x%04lx\n",hmem,procID);
        return GlobalLock(hmem);
}
/*************************************************************************
 * SHUnlockShared                               [SHELL32.522]
 *
 * NOTES
 *  parameter1 is return value from SHLockShared
 */
BOOL WINAPI SHUnlockShared(HANDLE pmem)
{       TRACE("handle=0x%04x\n",pmem);
        return GlobalUnlock(pmem);
}
/*************************************************************************
 * SHFreeShared                                 [SHELL32.523]
 *
 * NOTES
 *  parameter1 is return value from SHAllocShared
 *  parameter2 is return value from GetCurrentProcessId
 */
HANDLE WINAPI SHFreeShared(HANDLE hmem, DWORD procID)
{       TRACE("handle=0x%04x 0x%04lx\n",hmem,procID);
        return GlobalFree(hmem);
}