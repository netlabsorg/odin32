/* $Id: k32SetEnvironment.cpp,v 1.2.2.1 2001-09-27 03:08:23 bird Exp $
 *
 * k32SetEnvironment - Sets the Odin32 environment for a process.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR
#define NO_WIN32K_LIB_FUNCTIONS
#define INCL_KKL_LOG
#define INCL_KKL_AVL


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include <memory.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"
#include "dev32hlp.h"
#include "PerTaskW32kData.h"



/**
 * Set the Odin32 environment pointer for a process.
 * @returns NO_ERROR on success.
 *          Appropriate error code on failure.
 * @param   pszzEnvironment Pointer to environment block for the process identified by pid.
 * @param   cchEnvironment  Size of the environment block.
 * @param   pid             Process Id to set environment for.
 *                          Currently limited to the current process (if pszzEnvironment != NULL).
 * @status  Completely implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  This is called by the kernel32 initterm proc.
 *          It should be called by any environment replacement later...
 */
APIRET k32SetEnvironment(PSZ pszzEnvironment, ULONG cchEnvironment, PID pid)
{
    APIRET  rc;
    PPTD    pptd;

    /*
     * Validate pid a little bit...
     */
    if (pid >= 0x10000)
    {
        kprintf(("k32SetEnvironment: invalid pid=%x\n", pid));
        return ERROR_INVALID_PARAMETER;
    }

    if (pid != 0)
    {
        kprintf(("k32SetEnvironment: currently only supported for current pid. pid=%x\n", pid));
        return ERROR_INVALID_PARAMETER;
    }


    /*
     * Take Loader semaphore as that currently protects everything in this driver...
     */
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("k32SetEnvironment: LDRRequestSem failed with rc = %d\n", rc));
        return rc;
    }


    /*
     * Get Per Task Data and try set next environment pointer.
     */
    pptd = GetTaskData(ptdaGetCur(), TRUE);
    if (pptd)
    {
        if (*(PULONG)&pptd->lockOdin32Env)
        {
            D32Hlp_VMUnLock(&pptd->lockOdin32Env);
            memset(&pptd->lockOdin32Env, 0, sizeof(pptd->lockOdin32Env));
        }
        pptd->pszzOdin32Env = NULL;
        if (pptd->cUsage != 0 && pszzEnvironment != NULL && cchEnvironment != 0)
        {
            /*
             * Lock the memory (so we don't block or trap during environment searchs).
             */
            rc = D32Hlp_VMLock2(pszzEnvironment, cchEnvironment,
                                VMDHL_LONG | VMDHL_WRITE, &pptd->lockOdin32Env);
            if (rc == NO_ERROR)
                pptd->pszzOdin32Env = pszzEnvironment;
            else
            {
                kprintf(("k32SetEnvironment: VMLock2 failed with rc=%d\n", rc));
                memset(&pptd->lockOdin32Env, 0, sizeof(pptd->lockOdin32Env));
            }
        }
        else
            rc = NO_ERROR;
    }
    else
        rc = ERROR_NOT_ENOUGH_MEMORY;

    LDRClearSem();
    return rc;
}

