/* $Id: myLDRQAppType.cpp,v 1.1.2.3 2002-04-01 09:57:16 bird Exp $
 *
 * _myLDRQAppType - _LDRQAppType overload.
 *
 * Copyright (c) 1998-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: myLDRQAppType.cpp,v 1.1.2.3 2002-04-01 09:57:16 bird Exp $";
#endif

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI

#define INCL_OS2KRNL_PTDA
#define INCL_OS2KRNL_SEM
#define INCL_OS2KRNL_LDR
#define INCL_KKL_LOG
#define INCL_KKL_AVL

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "ldr.h"


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
    KLOGENTRY2("ULONG","ULONG p1, ULONG p2", p1, p2);
    APIRET rc;

    rc = KSEMRequestMutex(ptda_ptda_ptdasem(ptdaGetCur()), KSEM_INDEFINITE_WAIT);
    if (rc != NO_ERROR)
    {
        kprintf(("myLDRQAppType: failed to get intra-process semaphore.\n"));
        KLOGEXIT(rc);
        return rc;
    }
    rc = LDRRequestSem();
    if (rc != NO_ERROR)
    {
        kprintf(("myLDRQAppType: failed to get loader semaphore.\n"));
        KLOGEXIT(rc);
        return rc;
    }

    ASSERT_LdrStateUnknown("myLDRQAppType")
    setLdrStateQAppType();

    rc = LDRQAppType(p1, p2);

    ASSERT_LdrStateQAppType("myLDRQAppType")
    setLdrStateUnknown();

    LDRClearSem();
    KSEMReleaseMutex(ptda_ptda_ptdasem(ptdaGetCur()));

    KLOGEXIT(rc);
    return rc;
}
