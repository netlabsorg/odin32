/* $Id: oslibmem.cpp,v 1.2 2002-07-13 16:30:40 sandervl Exp $ */
/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998-2002 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#define INCL_DOSEXCEPTIONS
#define INCL_DOSMEMMGR
#define INCL_DOSPROCESS
#define INCL_DOSFILEMGR
#define INCL_DOSERRORS
#define INCL_DOSDEVIOCTL
#define INCL_DOSDEVICES
#define INCL_NPIPES
#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <win32api.h>
#include <winconst.h>
#include <win\winioctl.h>
#include <misc.h>
#include "initterm.h"
#include "oslibdos.h"
#include "dosqss.h"
#include "win32k.h"

#define DBG_LOCALLOG    DBG_oslibmem
#include "dbglocal.h"

//******************************************************************************
//TODO: Assumes entire memory range has the same protection flags!
//TODO: Check if this works for code aliases...
//******************************************************************************
DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl)
{
    DWORD rc;
    DWORD attr;
    DWORD size = cb;

    cb = (cb-1) & ~0xfff;
    cb+= PAGE_SIZE;

    rc = DosQueryMem(pb, &size, &attr);
    if(rc) {
        dprintf(("OSLibDosAliasMem: DosQueryMem %x %x return %d", pb, size, rc));
        return rc;
    }
    size = (size-1) & ~0xfff;
    size+= PAGE_SIZE;
    if(size != cb) {
        dprintf(("ERROR: OSLibDosAliasMem: size != cb (%x!=%x)!!!!!!!!", size, cb));
        //ignore this and continue return 5;
        attr = fl; //just use original protection flags (NOT CORRECT)
    }
    attr &= (PAG_READ|PAG_WRITE|PAG_EXECUTE|PAG_GUARD|PAG_DEFAULT);
    if(attr != fl) {
        rc = DosSetMem(pb, size, fl);
        if(rc) {
                dprintf(("OSLibDosAliasMem: DosSetMem %x %x return %d", pb, size, rc));
                attr = fl;
                //just continue for now
                //return rc;
        }
    }
    rc = DosAliasMem(pb, cb, ppbAlias, 2);
    if(rc) {
        dprintf(("OSLibDosAliasMem: DosAliasMem %x %x returned %d", pb, cb, rc));
        return rc;
    }
    if(attr != fl) {
        rc = DosSetMem(pb, size, attr);
        if(rc) {
            dprintf(("OSLibDosAliasMem: DosSetMem (2) %x %x return %d", pb, size, rc));
            return rc;
        }
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD cbSize, DWORD flFlags)
{
    PVOID   pvMemAddr;
    DWORD   offset;
    APIRET  rc;

    /*
     * Let's try use the extended DosAllocMem API of Win32k.sys.
     */
    if (libWin32kInstalled())
    {
        rc = DosAllocMemEx(lplpMemAddr, cbSize, flFlags | flAllocMem | OBJ_ALIGN64K);
        if (rc != ERROR_NOT_SUPPORTED)  /* This call was stubbed until recently. */
            return rc;
    }

    /*
     * If no or old Win32k fall back to old method.
     */

    rc = DosAllocMem(&pvMemAddr, cbSize, flFlags | flAllocMem);
    if(rc) {
        dprintf(("DosAllocMem failed with rc %d", rc));
        return rc;
    }
    //TODO!!!!!!!!!!!!
#if 0
    // already 64k aligned ?
    if((DWORD) pvMemAddr & 0xFFFF) 
    {
        DWORD addr64kb;

        //Allocate 64kb more so we can round the address to a 64kb aligned value
        rc = DosAllocMem((PPVOID)&addr64kb, cbSize + 64*1024,  PAG_READ | flAllocMem);
        if(rc) {
            dprintf(("DosAllocMem failed with rc %d", rc));
            return rc;
        }
        if(addr64kb & 0xFFFF) {
            addr64kb = (addr64kb + 0xFFFF) & 0xFFFF;
        }
        pvMemAddr = (PVOID)addr64kb;

        //and set the correct page flags for the request range
        rc = DosSetMem(pvMemAddr, cbSize, flFlags | flAllocMem);
    }
#endif
    if(!rc)
        *lplpMemAddr = pvMemAddr;

    return rc;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFreeMem(LPVOID lpMemAddr)
{
    return DosFreeMem(lpMemAddr);
}
//******************************************************************************
//NOTE: If name == NULL, allocated gettable unnamed shared memory
//OS/2 returns error 123 (invalid name) if the shared memory name includes
//colons. We need to replace them with underscores.
//******************************************************************************
DWORD OSLibDosAllocSharedMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags, LPSTR name)
{
    APIRET rc;
    char  *sharedmemname = NULL, *tmp;

    if(name) {
         sharedmemname = (char *)malloc(strlen(name) + 16);
         strcpy(sharedmemname, "\\SHAREMEM\\");
         strcat(sharedmemname, name);
    }
    else flags |= OBJ_GETTABLE;

    //SvL: Colons are unacceptable in shared memory names (for whatever reason)
    tmp = sharedmemname;
    while(*tmp != 0) {
        if(*tmp == ':') {
            *tmp = '_';
        }
        tmp++;
    }

    rc = DosAllocSharedMem(lplpMemAddr, sharedmemname, size, flags);
    if(name) {
        free(sharedmemname);
    }
    return rc;
}
//******************************************************************************
//NOTE: If name == NULL, assume gettable unnamed shared memory
//******************************************************************************
DWORD OSLibDosGetNamedSharedMem(LPVOID *lplpMemAddr, LPSTR name)
{
    APIRET rc;
    char  *sharedmemname = NULL, *tmp;

    if(name) {
        sharedmemname = (char *)malloc(strlen(name) + 16);
        strcpy(sharedmemname, "\\SHAREMEM\\");
        strcat(sharedmemname, name);

        //SvL: Colons are unacceptable in shared memory names (for whatever reason)
        tmp = sharedmemname;
        while(*tmp != 0) {
            if(*tmp == ':') {
                *tmp = '_';
            }
            tmp++;
        }
        rc = DosGetNamedSharedMem(lplpMemAddr, sharedmemname, PAG_READ|PAG_WRITE);
        if(name) {
                free(sharedmemname);
        }
    }
    else  rc = DosGetSharedMem((LPVOID)*(DWORD *)lplpMemAddr, PAG_READ|PAG_WRITE);

    return rc;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr)
{
    return DosQueryMem(lpMemAddr, lpRangeSize, lpAttr);
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags)
{
    APIRET rc;

    rc = DosSetMem(lpMemAddr, size, flags);
    switch(rc) {
    case ERROR_INVALID_ADDRESS:
        return OSLIB_ERROR_INVALID_ADDRESS;
    case ERROR_ACCESS_DENIED:
        return OSLIB_ERROR_ACCESS_DENIED;
    default:
        return rc;
    }
}
//******************************************************************************
//******************************************************************************
