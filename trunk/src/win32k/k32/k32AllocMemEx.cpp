/* $Id: k32AllocMemEx.cpp,v 1.4 2000-12-11 06:53:52 bird Exp $
 *
 * k32AllocMemEx - Equivalent to DosAllocMem, but this one
 *                 uses the address in ppv.
 *
 * Copyright (c) 2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS
#define INCL_OS2KRNL_VM

#define NO_WIN32K_LIB_FUNCTIONS

#ifndef OBJ_SELMAPALL
#define OBJ_SELMAPALL   0x00000800UL
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "OS2Krnl.h"
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"
#include "log.h"
#include "macros.h"


/* nasty! These should be imported from the kernel later! */

ULONG vmApiF0[] =
{
    0,
    VMA_READ,
    VMA_WRITE | VMA_READ,
    VMA_WRITE | VMA_READ,
    VMA_EXECUTE,
    VMA_EXECUTE | VMA_READ,
    VMA_WRITE | VMA_EXECUTE | VMA_READ,
    VMA_WRITE | VMA_EXECUTE | VMA_READ,
    VMA_GUARD,
    VMA_READ | VMA_GUARD,
    VMA_WRITE | VMA_READ | VMA_GUARD,
    VMA_WRITE | VMA_READ | VMA_GUARD,
    VMA_EXECUTE | VMA_GUARD,
    VMA_EXECUTE | VMA_READ | VMA_GUARD,
    VMA_WRITE | VMA_EXECUTE | VMA_READ | VMA_GUARD,
    VMA_WRITE | VMA_EXECUTE | VMA_READ | VMA_GUARD
};


ULONG vmApiF1[] =
{
    0,
    0,
    VMA_DECOMMIT,
    VMA_DECOMMIT,
    VMA_TILE,
    VMA_TILE,
    VMA_DECOMMIT | VMA_TILE,
    VMA_DECOMMIT | VMA_TILE,
    VMA_PROTECTED,
    VMA_PROTECTED,
    VMA_PROTECTED | VMA_DECOMMIT,
    VMA_PROTECTED | VMA_DECOMMIT,
    VMA_PROTECTED | VMA_TILE,
    VMA_PROTECTED | VMA_TILE,
    VMA_PROTECTED | VMA_DECOMMIT | VMA_TILE,
    VMA_PROTECTED | VMA_DECOMMIT | VMA_TILE
};


/**
 * Extended edition of DosAllocMem. Allows you to suggest an address where you
 * want the memory allocated.
 * @returns   OS2 returncode.
 * @param     ppv    Pointer to a pointer. This points to a pointer which points to the
 *                   suggested address of the memory block.
 * @param     cb     Size of the memory block.
 * @param     flag   Flags just as for DosAllocMem.
 * @param     ulCS   CS where the memoryobject were requested.
 * @param     ulEIP  EIP where the memoryobject were requested.
 * @sketch
 * @status    stub
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET k32AllocMemEx(PPVOID ppv, ULONG cb, ULONG flag, ULONG ulCS, ULONG ulEIP)
{
    #if 0
    APIRET  rc;
    ULONG   flFlags2;
    VMAC    vmac = {0};
    ULONG   cbCommit;
    HMTE    hMTE;

    if ((flag & ~(PAG_READ | PAG_WRITE | PAG_EXECUTE | PAG_GUARD | PAG_COMMIT
                 | OBJ_TILE | OBJ_SELMAPALL))
        || (flag & (PAG_READ | PAG_WRITE | PAG_EXECUTE)) == 0UL
        || cb == 0
        || ulCS == 0
        || ulEIP == 0
        )
        return ERROR_INVALID_PARAMETER;
    if (cb > 0xC0000000UL)
        return ERROR_NOT_ENOUGH_MEMORY;

    vmac.ac_va = (ULONG)*ppv;
    cb = (cb + 0xFFFUL) & ~0xFFFUL;
    if (flag & PAG_COMMIT)
        cbCommit = cb;
    flag = vmApiF0[flag & (PAG_READ | PAG_WRITE | PAG_EXECUTE | PAG_GUARD)]
           | vmApiF1[(flag & (PAG_COMMIT | PAG_DECOMMIT | OBJ_TILE | OBJ_PROTECTED)) >> 2]
           | VMA_USER | VMA_RESIDENT | VMA_LOWMEM2 | VMA_TILE | 0x12000000;
    flFlags2 = (flag & VMA_WRITE ? VMAF2_WRITE : 0) | 0x460;

    hMTE = VMGetOwner(ulCS, ulEIP);
    rc = VMAllocMem(cb,
                    cbCommit,
                    flag,
                    0,
                    0,
                    hMTE,
                    flFlags2,
                    0,
                    (PVMAC)SSToDS(&vmac));

    return rc;
    #else
    NOREF(ppv);
    NOREF(cb);
    NOREF(flag);
    NOREF(ulCS);
    NOREF(ulEIP);
    return ERROR_NOT_SUPPORTED;
    #endif
}

