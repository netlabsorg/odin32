/* $Id: k32QuerySystemMemInfo.cpp,v 1.1 2001-02-11 15:22:18 bird Exp $
 *
 * k32QuerySystemMemInfo - Collects more or less useful information on the
 *                         memory state of the system.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define FOR_EXEHDR 1                    /* To make all object flags OBJ???. */
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS

#define INCL_OS2KRNL_PG
#define INCL_OS2KRNL_SM
#define INCL_OS2KRNL_VM
#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR

#define NO_WIN32K_LIB_FUNCTIONS

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>                        /* OS/2 header file. */
#include <peexe.h>                      /* Wine PE structs and definitions. */
#include <neexe.h>                      /* Wine NE structs and definitions. */
#include <newexe.h>                     /* OS/2 NE structs and definitions. */
#include <exe386.h>                     /* OS/2 LX structs and definitions. */

#include "devSegDf.h"                   /* Win32k segment definitions. */

#include "malloc.h"                     /* win32k malloc (resident). Not C library! */
#include "smalloc.h"                    /* win32k swappable heap. */
#include "rmalloc.h"                    /* win32k resident heap. */

#include <string.h>                     /* C library string.h. */
#include <stdlib.h>                     /* C library stdlib.h. */
#include <stddef.h>                     /* C library stddef.h. */
#include <stdarg.h>                     /* C library stdarg.h. */

#include "vprintf.h"                    /* win32k printf and vprintf. Not C library! */
#include "dev1632.h"                    /* Common 16- and 32-bit parts */
#include "dev32.h"                      /* 32-Bit part of the device driver. (SSToDS) */
#include "OS2Krnl.h"                    /* kernel structs.  (SFN) */
#include "log.h"                        /* Logging. */
#include "options.h"                    /* Win32k options. */

#include "ProbKrnl.h"                   /* ProbKrnl variables and definitions. */
#include "win32k.h"                     /* Win32k API structures.  */
#include "k32.h"                        /* Internal Win32k API structures.  */


/**
 * Collects more or less useful information on the memory state of the system.
 * @returns     OS2 returncode.
 * @param       pMemInfo    Pointer to options structure. (NULL is allowed)
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark      One of the pointer may be NULL.
 */
APIRET k32QuerySystemMemInfo(PK32SYSTEMMEMINFO pMemInfo)
{
    APIRET              rc;
    K32SYSTEMMEMINFO    MemInfo;

    /*
     * Validate parameters.
     *  Ensure that the buffer pointer is sensible.
     *  Ensure that the structure sizes are correct.
     */
    if ((ULONG)pMemInfo < 0x10000)
        rc = ERROR_INVALID_PARAMETER;

    rc = TKFuBuff(SSToDS(&MemInfo.cb), &pMemInfo->cb, sizeof(ULONG)*2, TK_FUSU_NONFATAL);
    if (rc)
        return rc;
    if (MemInfo.cb != sizeof(K32SYSTEMMEMINFO))
        return ERROR_INVALID_PARAMETER;
    if (MemInfo.flFlags > 7)
        return ERROR_INVALID_PARAMETER;


    /*
     * Find parameters.
     */
    ULONG   flFlags = !MemInfo.flFlags
                        ? (K32_SYSMEMINFO_SWAPFILE | K32_SYSMEMINFO_PAGING | K32_SYSMEMINFO_VM)
                        : MemInfo.flFlags;

    /*
     * Swap file stuff.
     */
    if (flFlags & K32_SYSMEMINFO_SWAPFILE)
    {
        MemInfo.fSwapFile               = SMswapping;
        MemInfo.cbSwapFileSize          = smFileSize << PAGESHIFT;
        MemInfo.cbSwapFileAvail         = SMcDFAvail << PAGESHIFT;
        MemInfo.cbSwapFileUsed          = SMcDFInuse << PAGESHIFT;
        MemInfo.cbSwapFileMinFree       = SMMinFree << PAGESHIFT;
        MemInfo.cbSwapFileCFGMinFree    = (SMCFGMinFree << PAGESHIFT) / 4;
        MemInfo.cbSwapFileCFGSwapSize   = (SMCFGSwapSize << PAGESHIFT) / 4;
        MemInfo.cSwapFileBrokenDF       = smcBrokenDF;
        MemInfo.cSwapFileGrowFails      = smcGrowFails;
        MemInfo.cSwapFileInMemFile      = SMcInMemFile;
    }


    /*
     * Physical and Paging stuff.
     */
    if (flFlags & K32_SYSMEMINFO_PAGING)
    {
        MemInfo.cbPhysSize              = pgPhysPages << PAGESHIFT;
        if (SMswapping)                 /* the 16-bit MemInfo uses this scheme. */
            MemInfo.cbPhysAvail         = PGPhysAvail();
        else
        {
            int cPages = (pgPhysPages - pgResidentPages - pgSwappablePages - pgDiscardablePages);
            MemInfo.cbPhysAvail = (cPages > 0) ? cPages << PAGESHIFT : 0;
        }
        MemInfo.cbPhysUsed              = PGPhysPresent() << PAGESHIFT;
        MemInfo.fPagingSwapEnabled      = PGSwapEnabled;
        MemInfo.cPagingPageFaults       = pgcPageFaults;
        MemInfo.cPagingPageFaultsActive = pgcPageFaultsActive;
        MemInfo.cPagingPhysPages        = pgPhysPages;
        MemInfo.ulPagingPhysMax         = pgPhysMax;
        MemInfo.cPagingResidentPages    = pgResidentPages;
        MemInfo.cPagingSwappablePages   = pgSwappablePages;
        MemInfo.cPagingDiscardableInmem = pgDiscardableInmem;
        MemInfo.cPagingDiscardablePages = pgDiscardablePages;
    }


    /*
     * Virtual Memory stuff.
     */
    if (flFlags & K32_SYSMEMINFO_VM)
    {
        MemInfo.ulAddressLimit          = VirtualAddressLimit;
        vmRecalcShrBound(VMRSBF_ARENASHR, (PULONG)SSToDS(&MemInfo.ulVMArenaPrivMax));
        MemInfo.ulVMArenaSharedMin      = pahvmShr->ah_laddrMin;
        MemInfo.ulVMArenaSharedMax      = pahvmShr->ah_laddrMax;
        MemInfo.ulVMArenaSystemMin      = pahvmSys->ah_laddrMin;
        MemInfo.ulVMArenaSystemMax      = pahvmSys->ah_laddrMax;
        if (pahvmhShr)
        {
            MemInfo.ulVMArenaHighSharedMin  = pahvmhShr->ah_laddrMin;
            MemInfo.ulVMArenaHighSharedMax  = pahvmhShr->ah_laddrMax;
            vmRecalcShrBound(VMRSBF_ARENAHIGH, (PULONG)SSToDS(&MemInfo.ulVMArenaHighPrivMax));
        }
        else
            MemInfo.ulVMArenaHighPrivMax = MemInfo.ulVMArenaHighSharedMin = MemInfo.ulVMArenaHighSharedMax = 0;
    }

    /*
     * Now we're finished - so copy the data into user space and return.
     */
    rc = TKSuBuff(pMemInfo, SSToDS(&MemInfo), MemInfo.cb, TK_FUSU_NONFATAL);
    if (rc)
        kprintf(("k32QuerySystemMemInfo: Failed to copy meminfo to user memory. rc=%d\n", rc));

    return rc;
}

