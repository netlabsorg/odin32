/* $Id: d32Win32kOpenClose.c,v 1.1.2.2 2002-04-01 09:06:03 bird Exp $
 *
 * Open and Close handlers for the Win32k driver.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: d32Win32kOpenClose.c,v 1.1.2.2 2002-04-01 09:06:03 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR
#define INCL_OS2KRNL_PTDA
#define INCL_KKL_AVL
#define INCL_KKL_LOG

#define NO_WIN32K_LIB_FUNCTIONS


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"
#include "dev32hlp.h"
#include "dev1632.h"
#include "dev32.h"
#include "Win32k.h"
#include "PerTaskW32kData.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/


/**
 * Open handler.
 * Make sure there is a task data structure.
 * @returns Strategy return status.
 * @param   pRpOpen     Pointer to request packet.
 * @author  knut st. osmundsen (kosmunds@csc.no)
 */
USHORT _loadds _Far32 _Pascal Win32kOpen(PRP32OPENCLOSE pRpOpen)
{
    KLOGENTRY1("USHORT","PRP32OPENCLOSE pRpOpen", pRpOpen);
    APIRET  rc;
    PPTD    pptd;

    /*
     * Take Loader semaphore as that currently protects everything in this driver...
     */
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("Win32kOpen: LDRRequestSem failed with rc = %d\n", rc));
        //return rc;
    }

    pptd = GetTaskData(0, TRUE);
    if (pptd)
        pptd->cUsage++;

    pRpOpen = pRpOpen;
    LDRClearSem();
    KLOGEXIT(STATUS_DONE);
    return STATUS_DONE;
}


/**
 * Close handler.
 * Cleanup task data structure.
 * @returns Strategy return status.
 * @param   pRpOpen     Pointer to request packet.
 * @author  knut st. osmundsen (kosmunds@csc.no)
 */
USHORT _loadds _Far32 _Pascal Win32kClose(PRP32OPENCLOSE pRpClose)
{
    KLOGENTRY1("USHORT","PRP32OPENCLOSE pRpClose", pRpClose);
    APIRET  rc;
    PPTD    pptd;

    /*
     * Take Loader semaphore as that currently protects everything in this driver...
     */
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("Win32kClose: LDRRequestSem failed with rc = %d\n", rc));
        //return rc;
    }

    pptd = GetTaskData(0, FALSE);
    if (pptd)
    {
        if (pptd->cUsage > 0)
            pptd->cUsage--;
        if (pptd->cUsage == 0)
            RemoveTaskData(0);
    }
    pRpClose = pRpClose;

    LDRClearSem();
    KLOGEXIT(STATUS_DONE);
    return STATUS_DONE;
}

