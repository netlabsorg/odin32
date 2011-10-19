/* $Id: oslibmem.cpp,v 1.7 2003-03-27 14:00:53 sandervl Exp $ */
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
#include <win/winioctl.h>
#include <dbglog.h>
#include <vmutex.h>
#include "initterm.h"
#include "oslibdos.h"
#include "oslibmem.h"
#include "dosqss.h"
#include "win32k.h"
#include "exceptstackdump.h"

#define DBG_LOCALLOG    DBG_oslibmem
#include "dbglocal.h"

#include <_ras.h>

#ifdef RAS
RAS_TRACK_HANDLE rthVirtual = 0;

void rasInitVirtual (void)
{
    RasRegisterObjectTracking (&rthVirtual, "Virtual* memory allocation",
                               0, RAS_TRACK_FLAG_MEMORY | RAS_TRACK_FLAG_LOG_OBJECTS_AT_EXIT,
                               NULL, NULL);
}
#endif

typedef struct _VirtAllocRec {
  ULONG baseaddr;
  ULONG size;
  ULONG attr;

  struct _VirtAllocRec *next;
} VirtAllocRec;

static VirtAllocRec         *allocrecords  = NULL;
static CRITICAL_SECTION_OS2  alloccritsect = {0};

//******************************************************************************
//******************************************************************************
void AddAllocRec(ULONG baseaddr, ULONG size, ULONG attr)
{
    VirtAllocRec *rec, *tmp;

    rec = (VirtAllocRec *)malloc(sizeof(VirtAllocRec));
    if(!rec) {
        DebugInt3();
        return;
    }
    rec->baseaddr = baseaddr;
    rec->size     = size;
    rec->attr     = attr;

    DosEnterCriticalSection(&alloccritsect);
    if(!allocrecords || allocrecords->baseaddr > baseaddr) {
        rec->next     = allocrecords;
        allocrecords  = rec;
    }
    else {
        tmp = allocrecords;
        while(tmp->next) {
            if(tmp->next->baseaddr > baseaddr) {
                break;
            }
            tmp = tmp->next;
        }

        rec->next = tmp->next;
        tmp->next = rec;
    }
    DosLeaveCriticalSection(&alloccritsect);
}
//******************************************************************************
//******************************************************************************
void FreeAllocRec(ULONG baseaddr)
{
    VirtAllocRec *rec = NULL, *tmp;

    if(!allocrecords) {
        DebugInt3();
        return;
    }

    DosEnterCriticalSection(&alloccritsect);
    if(allocrecords->baseaddr == baseaddr) {
        rec          = allocrecords;
        allocrecords = allocrecords->next;
    }
    else {
        tmp = allocrecords;
        while(tmp->next) {
            if(tmp->next->baseaddr == baseaddr) {
                break;
            }
            tmp = tmp->next;
        }
        if(tmp->next) {
             rec = tmp->next;
             tmp->next = tmp->next->next;
        }
        else dprintf(("ERROR: FreeAllocRec: allocation not found!! (%x)", baseaddr));
    }
    DosLeaveCriticalSection(&alloccritsect);
    if(rec) free(rec);
}
//******************************************************************************
//******************************************************************************
BOOL FindAllocRec(ULONG addr, ULONG *lpBase, ULONG *lpSize, ULONG *lpAttr)
{
    VirtAllocRec *rec = NULL;

    DosEnterCriticalSection(&alloccritsect);
    rec = allocrecords;
    while(rec) {
        if(rec->baseaddr <= addr && rec->baseaddr + rec->size > addr) {
            *lpBase = rec->baseaddr;
            *lpSize = rec->size;
            *lpAttr = rec->attr;
            break; //found it
        }
        if(rec->baseaddr > addr) {
            //sorted list, so no need to search any further
            rec = NULL;
            break;
        }
        rec = rec->next;
    }
    DosLeaveCriticalSection(&alloccritsect);
    return (rec != NULL);
}
//******************************************************************************
//TODO: Check if this works for code aliases...
//NOTE: DosAliasMem fails if the address isn't page aligned
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
    AddAllocRec((ULONG)*ppbAlias, cb, fl);
    return 0;
}

//***************************************************************************
//Allocation memory at address helper
//***************************************************************************
#define OBJ_LOCSPECIFIC         0x00001000UL

int allocAtAddress(void *pvReq, ULONG cbReq, ULONG fReq)
{
    dprintf(("DosAllocMemEx pvReq=%p cbReq=%lu fReq=%#lx\n", pvReq, cbReq, fReq));
    PVOID           apvTmps[3000];
    ULONG           cbTmp;
    ULONG           fTmp;
    int             iTmp;
    int             rcRet = ERROR_NOT_ENOUGH_MEMORY;

    /*
     * Adjust flags and size.
     */
    if ((ULONG)pvReq < 0x20000000 /*512MB*/)
        fReq &= ~OBJ_ANY;
    else
        fReq |= OBJ_ANY;
    cbReq = (cbReq + 0xfff) & ~0xfff;

    /*
     * Allocation loop.
     * This algorithm is not optimal!
     */
    fTmp  = fReq & ~(PAG_COMMIT);
    cbTmp = 1*1024*1024; /* 1MB*/
    for (iTmp = 0; iTmp < sizeof(apvTmps) / sizeof(apvTmps[0]); iTmp++)
    {
        PVOID   pvNew = NULL;
        int     rc;

        /* Allocate chunk. */
        rc = DosAllocMem(&pvNew, cbReq, fReq);
        apvTmps[iTmp] = pvNew;
        if (rc)
            break;

        /*
         * Passed it?
         * Then retry with the requested size.
         */
        if (pvNew > pvReq)
        {
            if (cbTmp <= cbReq)
                break;
            DosFreeMem(pvNew);
            cbTmp = cbReq;
            iTmp--;
            continue;
        }

        /*
         * Does the allocated object touch into the requested one?
         */
        if ((char *)pvNew + cbTmp > (char *)pvReq)
        {
            /*
             * Yes, we've found the requested address!
             */
            apvTmps[iTmp] = NULL;
            DosFreeMem(pvNew);

            /*
             * Adjust the allocation size to fill the gap between the
             * one we just got and the requested one.
             * If no gap we'll attempt the real allocation.
             */
            cbTmp = (ULONG)pvReq - (ULONG)pvNew;
            if (cbTmp)
            {
                iTmp--;
                continue;
            }

            rc = DosAllocMem(&pvNew, cbReq, fReq);
            if (rc || (char *)pvNew > (char *)pvReq)
                break; /* we failed! */
            if (pvNew == pvReq)
            {
                rcRet = 0;
                break;
            }

            /*
             * We've got an object which start is below the one we
             * requested. This is probably caused by the requested object
             * fitting in somewhere our tmp objects didn't.
             * So, we'll have loop and retry till all such holes are filled.
             */
            apvTmps[iTmp] = pvNew;
        }
    }

    /*
     * Cleanup reserved memory and return.
     */
    while (iTmp-- > 0)
        if (apvTmps[iTmp])
            DosFreeMem(apvTmps[iTmp]);

    return rcRet;
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
#ifdef RAS
        if (rc == NO_ERROR)
        {
            RasAddObject (rthVirtual, (ULONG)*lplpMemAddr, NULL, cbSize);
        }
#endif
        if (rc != ERROR_NOT_SUPPORTED)  /* This call was stubbed until recently. */
            return rc;
    }

    /*
     * If no or old Win32k fall back to old method.
     */

    if (flFlags & OBJ_LOCSPECIFIC)
    {
        rc = allocAtAddress(&pvMemAddr, cbSize, (flFlags & ~OBJ_LOCSPECIFIC) | fMemFlags);
    } else
    {
        rc = DosAllocMem(&pvMemAddr, cbSize, flFlags | fMemFlags);
    }
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

        PVOID baseAddr = (PVOID)addr64kb; // sunlover20040613: save returned address for a possible Free on failure

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
                DosFreeMem (baseAddr); // sunlover20040613: Free allocated memory
                return rc;
            }
        }
    }

    if(!rc) {
        *lplpMemAddr = pvMemAddr;
        AddAllocRec((ULONG)pvMemAddr, cbSize, flFlags);
        RasAddObject (rthVirtual, (ULONG)*lplpMemAddr, NULL, cbSize);
    }
    return rc;
}
//******************************************************************************
//Locate the base page of a memory allocation (the page with the PAG_BASE attribute)
//******************************************************************************
PVOID OSLibDosFindMemBase(LPVOID lpMemAddr, DWORD *lpAttr)
{
    ULONG  ulAttr, ulSize, ulAddr, ulBase;
    APIRET rc;
    VirtAllocRec *allocrec;

    *lpAttr = 0;

    if(FindAllocRec((ULONG)lpMemAddr, &ulBase, &ulSize, lpAttr) == TRUE) {
        return (PVOID)ulBase;
    }

    ulSize = PAGE_SIZE;
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
    int i;
    for(i=0;i<16;i++) {
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
        return ERROR_INVALID_PARAMETER;
    }
    FreeAllocRec((ULONG)lpMemAddr);

    RasRemoveObject (rthVirtual, (ULONG)lpMemAddr);

    return DosFreeMem((PVOID)ulAddr);
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
