/* $Id: k32AllocMemEx.cpp,v 1.5.2.1 2001-09-27 03:08:21 bird Exp $
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
#define INCL_OS2KRNL_TK
#define NO_WIN32K_LIB_FUNCTIONS
#define INCL_KKL_LOG


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"


/* nasty! These should be imported from the kernel later! */
/*
 * These are unchanged from Warp3 to Warp 4.51.
 */
ULONG vmApiF0[] =
{
    0,                                              /* 0 */
    VMA_READ,                                       /* PAG_READ  */
    VMA_WRITE | VMA_READ,                           /* PAG_WRITE */
    VMA_WRITE | VMA_READ,                           /* PAG_READ | PAG_WRITE */
    VMA_EXECUTE,                                    /* PAG_EXECUTE */
    VMA_EXECUTE | VMA_READ,                         /* PAG_READ | PAG_EXECUTE */
    VMA_WRITE | VMA_EXECUTE | VMA_READ,             /* PAG_WRITE | PAG_EXECUTE */
    VMA_WRITE | VMA_EXECUTE | VMA_READ,             /* PAG_READ | PAG_WRITE | PAG_EXECUTE */
    VMA_GUARD,                                      /* PAG_GUARD  */
    VMA_READ | VMA_GUARD,                           /* PAG_READ | PAG_GUARD */
    VMA_WRITE | VMA_READ | VMA_GUARD,               /* PAG_WRITE | PAG_GUARD */
    VMA_WRITE | VMA_READ | VMA_GUARD,               /* PAG_READ | PAG_WRITE | PAG_GUARD */
    VMA_EXECUTE | VMA_GUARD,                        /* PAG_EXECUTE | PAG_GUARD */
    VMA_EXECUTE | VMA_READ | VMA_GUARD,             /* PAG_READ | PAG_EXECUTE | PAG_GUARD */
    VMA_WRITE | VMA_EXECUTE | VMA_READ | VMA_GUARD, /* PAG_WRITE | PAG_EXECUTE | PAG_GUARD */
    VMA_WRITE | VMA_EXECUTE | VMA_READ | VMA_GUARD  /* PAG_READ | PAG_WRITE | PAG_EXECUTE | PAG_GUARD */
};

ULONG vmApiF1[] =
{
    0,                                              /* 0 */
    0,                                              /* PAG_COMMIT */
    VMA_DECOMMIT,                                   /* PAG_DECOMMIT */
    VMA_DECOMMIT,                                   /* PAG_COMMIT | PAG_DECOMMIT */
    VMA_SELALLOC,                                   /* OBJ_TILE */
    VMA_SELALLOC,                                   /* PAG_COMMIT | OBJ_TILE */
    VMA_DECOMMIT | VMA_SELALLOC,                    /* PAG_DECOMMIT | OBJ_TILE */
    VMA_DECOMMIT | VMA_SELALLOC,                    /* PAG_COMMIT | PAG_DECOMMIT | OBJ_TILE */
    VMA_PROTECTED,                                  /* OBJ_PROTECTED */
    VMA_PROTECTED,                                  /* PAG_COMMIT | OBJ_PROTECTED */
    VMA_PROTECTED | VMA_DECOMMIT,                   /* PAG_DECOMMIT | OBJ_PROTECTED */
    VMA_PROTECTED | VMA_DECOMMIT,                   /* PAG_COMMIT | PAG_DECOMMIT | OBJ_PROTECTED */
    VMA_PROTECTED | VMA_SELALLOC,                   /* OBJ_TILE | OBJ_PROTECTED */
    VMA_PROTECTED | VMA_SELALLOC,                   /* PAG_COMMIT | OBJ_TILE | OBJ_PROTECTED */
    VMA_PROTECTED | VMA_DECOMMIT | VMA_SELALLOC,    /* PAG_DECOMMIT | OBJ_TILE | OBJ_PROTECTED */
    VMA_PROTECTED | VMA_DECOMMIT | VMA_SELALLOC     /* PAG_COMMIT | PAG_DECOMMIT | OBJ_TILE | OBJ_PROTECTED */
};


/**
 * Extended edition of DosAllocMem:
 *      Allows you to suggest an address where you want the memory allocated.
 *      Specify 64KB alignment.
 * @returns OS2 returncode.
 *          ERROR_INVALID_PARAMETER   Some input parameter is bad.
 *          ERROR_INVALID_ADDRESS     *pvv is wrong.
 * @param   ppv     Pointer to a pointer.
 *                  If OBJ_LOCATION is specified this *ppv holds a suggested address of the memory block.
 * @param   cb      Size of the memory block.
 * @param   flFlags Flags just as for DosAllocMem and two more:
 *                      OBJ_ALIGN64K    Align on 64 boundrary. Default in compatibility region.
 *                      OBJ_ALIGNPAGE   Page align the memory. (only high region) Default in 32-bit region.
 *                      OBJ_LOCATION    *ppv specifies a suggested address of the memory block.
 * @param   ulCS    CS where the memoryobject was requested.
 * @param   ulEIP   EIP where the memoryobject was requested.
 * @sketch
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Seems like I have to solve that "Grow private arena" problem which
 *          applies when we allocate at a specific address.
 *          I'll look into it ASAP and add a fix for this in the loader too.
 */
APIRET k32AllocMemEx(PPVOID ppv, ULONG cb, ULONG flFlags, ULONG ulCS, ULONG ulEIP)
{
    APIRET  rc;
    ULONG   flVMFlags;
    ULONG   flSelFlags;
    VMAC    vmac = {0};
    ULONG   cbCommit;
    HMTE    hMTE;


    kprintf(("k32AllocMemEx(ppv=0x%08x, cb=0x%x, flFlags=0x%08x, ulCS=0x%04x, ulEIP=0x%08x)\n",
             ppv, cb, flFlags, ulCS, ulEIP));

    /** @sketch
     * General parameter validations:
     *      Only allowable flFlagss - DosAllocMem + our own.
     *      At least one access flag must be set.
     *      OBJ_TILE don't work OBJ_ANY or OBJ_ALIGNPAGE.
     *      Must be larger than 0 bytes.
     *      Caller CS and EIP must not be 0.
     * Align size and check if it's resonable.
     * If address is specified Then
     *      Check that the memory block is within possible private address space.
     */
    if (   (flFlags & ~(PAG_READ | PAG_WRITE | PAG_EXECUTE | PAG_GUARD | PAG_COMMIT
                        | OBJ_ANY | OBJ_TILE | OBJ_SELMAPALL | OBJ_ALIGNMASK | OBJ_LOCATION))
        || (flFlags & (PAG_READ | PAG_WRITE | PAG_EXECUTE)) == 0
        || ((flFlags & OBJ_TILE) && ((flFlags & OBJ_ANY) || (flFlags & OBJ_ALIGNMASK) == OBJ_ALIGNPAGE))
        || cb == 0
        || ulCS == 0 || ulEIP == 0
        )
    {
        kprintf(("k32AllocMemEx: Bad param (1)\n"));
        return ERROR_INVALID_PARAMETER;
    }

    cb = PAGEALIGNUP(cb);
    if (cb >= VirtualAddressLimit)
    {
        kprintf(("k32AllocMemEx: Bad size cb=%d\n", cb));
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    if (flFlags & OBJ_LOCATION)
    {
        rc = TKFuULongNF(SSToDS(&vmac.ac_va), ppv);
        if (rc)
        {
            kprintf(("k32AllocMemEx: Failed to fetch *ppv. rc=%d\n", rc));
            return rc;
        }
        kprintf(("k32AllocMemEx: Location *ppv =0x%08x\n", vmac.ac_va));
        if (    vmac.ac_va < 0x10000
            ||  vmac.ac_va + cb < vmac.ac_va
            ||  vmac.ac_va + cb > ((vmac.ac_va >= 0x20000000 && &ahvmhShr) ? ahvmhShr.ah_laddrMax : ahvmShr.ah_laddrMax)
                )
        {
            kprintf(("k32AllocMemEx: *ppv has an invalid address. *ppv=0x%08x\n", vmac.ac_va));
            return ERROR_INVALID_ADDRESS;
        }
    }


    /** @sketch
     * Get the hMTE of the executable object we're called from - may fail.
     * Determin size of precommitted memory.
     * Determin VM Flags:
     *      Default
     *      Handle OBJ_ANY (only applies if there are high arenas).
     *      Access flags are determined using vmApiF0.
     *      Commit, decommit, tile and unused protected flags using vmApiF1.
     * Set Alignment flags if specified.
     * If location specified Then Set location specific and arena flags correctly.
     * Else set the location any flag.
     * Determin SEL Flags.
     */
    hMTE = VMGetOwner(ulCS, ulEIP);

    cbCommit = flFlags & PAG_COMMIT ? cb : 0;

    flVMFlags =  VMA_USER | VMA_ZEROFILL | VMA_LOWMEM2 | VMA_ARENAPRIVATE /* 0x02010084*/
        | ((flFlags & OBJ_ANY) && &ahvmhShr ? VMA_ARENAHIGHA : VMA_SELALLOC | VMA_ALIGNSEL)
        | vmApiF0[flFlags  & (PAG_READ | PAG_WRITE | PAG_EXECUTE | PAG_GUARD)]
        | vmApiF1[(flFlags & (PAG_COMMIT | PAG_DECOMMIT | OBJ_TILE | OBJ_PROTECTED)) >> 4];

    if (    (flVMFlags & VMA_SELALLOC)
        ||  (flFlags & OBJ_ALIGNMASK) == OBJ_ALIGN64K)
        flVMFlags |= VMA_ALIGNSEL;
    else if ((flFlags & OBJ_ALIGNMASK) == OBJ_ALIGNPAGE)
        flVMFlags |= VMA_ALIGNPAGE;

    if (flFlags & OBJ_LOCATION)
    {
        flVMFlags |= VMA_LOCSPECIFIC;
        if (vmac.ac_va < 0x20000000)
            flVMFlags &= ~VMA_ARENAHIGHA;
    }
    else
        flVMFlags |= VMA_LOCANY;

    flSelFlags = 0;
    if (flVMFlags & VMA_SELALLOC)
        flSelFlags = SELAF_SELMAP | SELAF_DPL3 | /* 0x0460 */
                     (flVMFlags & VMA_WRITE ? SELAF_WRITE : 0);


    /** @sketch
     * Call VMAllocMem.
     * If success Then
     *      Try set return variable.
     *      Free Memory on failure and return rc from previous operation.
     * Endif
     * Return return code.
     */
    kprintf(("k32AllocMemEx: cb=0x%08x flVMFlags=0x%08x flSelFlags=0x%08x, hMTE=%04d vmac.ac_va=0x%08x\n",
             cb, flVMFlags, flSelFlags, hMTE, vmac.ac_va));
    rc = VMAllocMem(cb,
                    cbCommit,
                    flVMFlags,
                    0,
                    0,
                    hMTE,
                    flSelFlags,
                    0,
                    (PVMAC)SSToDS(&vmac));

    if (rc == NO_ERROR)
    {
        rc = TKSuULongNF(ppv, SSToDS(&vmac.ac_va));
        if (rc)
        {
            kprintf(("k32AllocMemEx: Failed to set *ppv. rc=%d\n", rc));
            VMFreeMem(vmac.ac_va, 0, 0);
        }
    }
    else
        kprintf(("k32AllocMemEx: VMAllocMem failed with rc=%d\n", rc));

    kprintf(("k32AllocMemEx: returns rc=%d (*ppv=0x%08x)\n", rc, vmac.ac_va));
    return rc;
}

