/* $Id: d32Win32kOpenClose.c,v 1.1 2001-07-08 02:53:18 bird Exp $
 *
 * Open and Close handlers for the Win32k driver.
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
#define INCL_NOPMAPI
#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_PTDA

#define NO_WIN32K_LIB_FUNCTIONS


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"
#include "dev32hlp.h"
#include "dev1632.h"
#include "dev32.h"
#include "OS2Krnl.h"
#include "Win32k.h"
#include "log.h"
#include "asmutils.h"
#include "avl.h"
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
    PPTD pptd = GetTaskData(0);
    if (pptd)
        pptd->cUsage++;

    pRpOpen = pRpOpen;
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
    PPTD pptd = GetTaskData(0);
    if (pptd)
    {
        if (pptd->cUsage > 0)
        {
            APIRET  rc;
            if (    pptd->pszzOdin32Env != NULL
                && (rc = D32Hlp_VMUnLock(&pptd->lockOdin32Env)) != NO_ERROR
                )
            {
                kprintf(("Win32kClose: VMUnLock failed with rc=%d\n", rc));
            }
            pptd->cUsage--;
        }
        if (pptd->cUsage == 0)
            RemoveTaskData(0);
    }
    pRpClose = pRpClose;
    return STATUS_DONE;
}

