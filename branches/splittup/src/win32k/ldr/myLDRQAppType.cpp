/* $Id: myLDRQAppType.cpp,v 1.5 2001-02-10 11:11:45 bird Exp $
 *
 * _myLDRQAppType - _LDRQAppType overload.
 *
 * Copyright (c) 1998-1999 knut  St.  osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "OS2Krnl.h"
#include "avl.h"
#include "ldr.h"
#include "log.h"


/**
 * LDRQAppType - Loader Query Application Type - DosQueryAppType worker.
 *
 * We overrides this be able to determin if a convertion is only for a
 * LDRQAppType call.
 *
 * isLdrStateQAppType() returns TRUE when this procedure is on the stack.
 *
 * @returns   return code.
 * @param     p1
 * @param     p2
 */
ULONG LDRCALL myLDRQAppType(ULONG p1, ULONG p2)
{
    APIRET rc;

    kprintf(("myLDRQAppType: entry\n"));
    rc = KSEMRequestMutex(ptda_ptda_ptdasem(ptdaGetCur()), KSEM_INDEFINITE_WAIT);
    if (rc != NO_ERROR)
    {
        kprintf(("myLDRQAppType: failed to get intra-process semaphore.\n"));
        return rc;
    }
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("myLDRQAppType: failed to get loader semaphore.\n"));
        return rc;
    }

    ASSERT_LdrStateUnknown("myLDRQAppType")
    setLdrStateQAppType();

    rc = LDRQAppType(p1, p2);

    ASSERT_LdrStateQAppType("myLDRQAppType")
    setLdrStateUnknown();

    LDRClearSem();
    KSEMReleaseMutex(ptda_ptda_ptdasem(ptdaGetCur()));
    kprintf(("myLDRQAppType: exit\n"));

    return rc;
}
