/* $Id: k32ProcessReadWrite.cpp,v 1.2.2.1 2001-09-27 03:08:22 bird Exp $
 *
 * k32ProcessReadWrite  -  Read or write to another process.
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

#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_VM
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR
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


/**
 * Reads or write memory in another process.
 * @returns     OS2 returncode.
 * @param       pid         Process ID which is to be written to.
 * @param       cb          Number of bytes to write.
 * @param       pvSource    Pointer to data to read.
 * @param       pvTarget    Pointer to where to write.
 * @param       fRead       TRUE:   pvSource is within pid while pvTarget is ours.
 *                          FALSE:  pvTarget is within pid while pvSource is ours.
 * @status      completely implelemented.
 * @author      knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark
 */
APIRET k32ProcessReadWrite(PID pid, ULONG cb, PVOID pvSource, PVOID pvTarget, BOOL fRead)
{
    ULONG   ulAddrAlias;
    ULONG   cbAlias;
    APIRET  rc;
    PPTDA   pPTDA;
    HPTDA   hPTDA;
    VMAC    vmac;

    /*
     * Find alias address and size and validate them a little.
     */
    ulAddrAlias = (ULONG)(fRead ? pvSource : pvTarget) & ~0xfffUL;
    cbAlias = (cb + 0xfffUL) & ~0xfffUL;
    if (cbAlias - 1 + ulAddrAlias < ulAddrAlias)
        return ERROR_INVALID_ACCESS;

    /*
     * Find the PTDA for the given PID.
     */
    pPTDA = NULL;
    rc = TKPidToPTDA(pid, (PPPTDA)SSToDS(&pPTDA));
    if (rc != NO_ERROR || pPTDA == NULL ||
        (hPTDA = ptdaGet_ptda_handle(pPTDA)) == 0 || hPTDA == (USHORT)-1
        )
    {
        kprintf(("k32ProcessReadWrite: Failed with invalid PID.\n"));
        return rc != NO_ERROR ? rc : ERROR_INVALID_PARAMETER;
    }

    /*
     * Take loader semaphore before calling VMMapDebugAlias.
     */
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("k32ProcessReadWrite: LDRRequestSem failed with rc=%d\n", rc));
        return rc;
    }

    /*
     * Allocate alias.
     */
    rc = VMMapDebugAlias(VMMDA_ARENASYSTEM | (fRead ? VMMDA_READONLY : 0),
                         ulAddrAlias,
                         cbAlias,
                         hPTDA,
                         (PVMAC)SSToDS(&vmac));
    LDRClearSem();
    if (rc != NO_ERROR)
    {
        kprintf(("k32ProcessReadWrite: VMMapDebugAlias failed with rc=%d\n"));
        return rc;
    }

    /*
     * Copy data.
     */
    if (fRead)
        pvSource = (PVOID)(vmac.ac_va + ((ULONG)pvSource & 0xfff));
    else
        pvTarget = (PVOID)(vmac.ac_va + ((ULONG)pvTarget & 0xfff));
    rc = TKSuFuBuff(pvTarget,
                    pvSource,
                    cb,
                    TK_FUSU_NONFATAL | TK_FUSU_USER_SRC | TK_FUSU_USER_DST);
    if (rc != NO_ERROR)
        kprintf(("k32ProcessReadWrite: TKSuFuBuff failed with rc=%d\n"));

    /*
     * Cleanup and exit.
     */
    pPTDA = ptdaGetCur();
    if (pPTDA == NULL || (hPTDA = ptdaGet_ptda_handle(pPTDA)) == 0 || hPTDA == (USHORT)-1)
        kprintf(("k32ProcessReadWrite: Failed to get hPTDA of current process\n"));
    if (VMFreeMem(vmac.ac_va, hPTDA, 0))
        kprintf(("k32ProcessReadWrite: VMFreeMem failed!\n"));

    return rc;
}


