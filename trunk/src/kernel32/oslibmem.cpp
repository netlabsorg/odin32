/* $Id: oslibmem.cpp,v 1.6 2003-03-06 10:44:34 sandervl Exp $ */
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
#include "oslibmem.h"
#include "dosqss.h"
#include "win32k.h"

#define DBG_LOCALLOG    DBG_oslibmem
#include "dbglocal.h"

//******************************************************************************
//TODO: Check if this works for code aliases...
//******************************************************************************
DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl)
{
    DWORD rc;
    DWORD attr;
    DWORD size = cb;

    cb = (cb-1) & ~0xfff;
    cb+= PAGE_SIZE;

    rc = DosAliasMem(pb, cb, ppbAlias, 2);
    if(rc) {
        dprintf(("!ERROR!: OSLibDosAliasMem: DosAliasMem %x %x returned %d", pb, cb, rc));
        return rc;
    }
    //Now try to change the protection flags of all pages in the aliased range
    DWORD pAlias = (DWORD)*ppbAlias;

    while(pAlias < (DWORD)*ppbAlias + cb) 
    {
        rc = DosQueryMem((PVOID)pAlias, &size, &attr);
        if(rc != NO_ERROR) {
            dprintf(("!ERROR!: OSLibDosAliasMem: DosQueryMem %x returned %d", pAlias, rc));
            DebugInt3();
            return rc;
        }
        //Don't bother if the pages are not committed. DosSetMem will return 
        //ERROR_ACCESS_DENIED. 
        if(attr & PAG_COMMIT) {
            rc = DosSetMem((PVOID)pAlias, size, fl);
            if(rc) {
                dprintf(("!ERROR!: OSLibDosAliasMem: DosSetMem %x %x return %d", *ppbAlias, size, rc));
                DebugInt3();
                return rc;
            }
        }
        pAlias += size;
    }
    return 0;
}
//******************************************************************************
//Allocate memory aligned at 64kb boundary
//******************************************************************************
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD cbSize, DWORD flFlags, BOOL fLowMemory)
{
    PVOID   pvMemAddr;
    DWORD   offset;
    APIRET  rc;
    BOOL    fMemFlags = flAllocMem;

    //Override low/high memory flag if necessary
    if(fLowMemory) {
        fMemFlags = 0;
    }
    /*
     * Let's try use the extended DosAllocMem API of Win32k.sys.
     */
    if (libWin32kInstalled())
    {
        rc = DosAllocMemEx(lplpMemAddr, cbSize, flFlags | fMemFlags | OBJ_ALIGN64K);
        if (rc != ERROR_NOT_SUPPORTED)  /* This call was stubbed until recently. */
            return rc;
    }

    /*
     * If no or old Win32k fall back to old method.
     */

    rc = DosAllocMem(&pvMemAddr, cbSize, flFlags | fMemFlags);
    if(rc) {
        dprintf(("!ERROR!: DosAllocMem failed with rc %d", rc));
        return rc;
    }
    // already 64k aligned ?
    if((ULONG) pvMemAddr & 0xFFFF) 
    {
        ULONG addr64kb;

        //free misaligned allocated memory
        DosFreeMem(pvMemAddr);

        //Allocate 64kb more so we can round the address to a 64kb aligned value
        rc = DosAllocMem((PPVOID)&addr64kb, cbSize + 64*1024,  (flFlags & ~PAG_COMMIT) | fMemFlags);
        if(rc) {
            dprintf(("!ERROR!: DosAllocMem failed with rc %d", rc));
            return rc;
        }
        dprintf(("Allocate aligned memory %x -> %x", addr64kb, (addr64kb + 0xFFFF) & ~0xFFFF));

        if(addr64kb & 0xFFFF) {
            addr64kb         = (addr64kb + 0xFFFF) & ~0xFFFF;
        }
        pvMemAddr = (PVOID)addr64kb;

        //and set the correct page flags for the request range
        if((flFlags & ~PAG_COMMIT) != flFlags) {
            rc = DosSetMem(pvMemAddr, cbSize, flFlags);
            if(rc) {
                dprintf(("!ERROR!: DosSetMem failed with rc %d", rc));
                return rc;
            }
        }
    }

    if(!rc)
        *lplpMemAddr = pvMemAddr;

    return rc;
}
//******************************************************************************
//Locate the base page of a memory allocation (the page with the PAG_BASE attribute)
//******************************************************************************
PVOID OSLibDosFindMemBase(LPVOID lpMemAddr)
{
    ULONG  ulAttr, ulSize, ulAddr;
    APIRET rc;
    
    rc = DosQueryMem(lpMemAddr, &ulSize, &ulAttr);
    if(rc != NO_ERROR) {
        dprintf(("!ERROR!: OSLibDosFindMemBase: DosQueryMem %x failed with rc %d", lpMemAddr, rc));
        return lpMemAddr;
    }
    if(!(ulAttr & PAG_BASE)) {
        //Not the base of the initial allocation (can happen due to alignment) or app
        //passing address inside memory allocation range
        ulAddr  = (DWORD)lpMemAddr & ~0xFFF;
        ulAddr -= PAGE_SIZE;

        while(ulAddr > 0)
        {
            rc = DosQueryMem((PVOID)ulAddr, &ulSize, &ulAttr);
            if(rc) {
                dprintf(("!ERROR!: OSLibDosFindMemBase: DosQueryMem %x failed with rc %d", lpMemAddr, rc));
                DebugInt3();
                return NULL;
            }
            if(ulAttr & PAG_BASE) {
                //Memory below the 512 MB boundary is always aligned at 64kb and VirtualAlloc only
                //returns high memory (if OS/2 version supports it)
                //If it is above the 512 MB boundary, then we must make sure the right base address
                //is returned. VirtualAlloc allocates extra memory to make sure it can return addresses
                //aligned at 64kb. If extra pages are needed, then the allocation base is inside
                //the filler region. In that case we must return the next 64kb address as base.
                if(ulAddr > MEM_TILED_CEILING) {
                    ulAddr = (ulAddr + 0xFFFF) & ~0xFFFF;
                }
                lpMemAddr = (PVOID)ulAddr;
                break;
            }
            ulAddr -= PAGE_SIZE;
        }
    }
    return lpMemAddr;
}
//******************************************************************************
//******************************************************************************
DWORD OSLibDosFreeMem(LPVOID lpMemAddr)
{
    ULONG  ulAttr, ulSize, ulAddr;
    APIRET rc;

    ulAddr = (DWORD)lpMemAddr & ~0xFFF;
    ulSize = 0x1000;

    //Find base within previous 64kb (alignment can add filler pages)
    for(int i=0;i<16;i++) {
        rc = DosQueryMem((PVOID)ulAddr, &ulSize, &ulAttr);
        if(rc != NO_ERROR) {
            dprintf(("!ERROR!: OSLibDosFreeMem: DosQueryMem %x failed with rc %d", lpMemAddr, rc));
            i = 16; //fail
            break;
        }
        if(ulAttr & PAG_BASE) {
            break;
        }
        ulAddr -= PAGE_SIZE;
    }
    if(i == 16) {
        //oh, oh. didn't find base; shouldn't happen!!
        dprintf(("!ERROR!: OSLibDosFreeMem: Unable to find base of %x", lpMemAddr));
        DebugInt3();
    }
    else {
        lpMemAddr = (PVOID)ulAddr;
    }
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
