/* $Id: myLDRQAppType.cpp,v 1.3 2000-01-22 18:21:02 bird Exp $
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

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "OS2Krnl.h"
#include "ldrCalls.h"
#include "log.h"

BOOL fQAppType = FALSE;

/**
 * LDRQAppType - Loader Query Application Type.
 * We set a flag while we're executing this function. Just to speed up processing.
 * If a PE file is queried, a dummy LX header is presented.
 * @returns   return code.
 * @param     p1
 * @param     p2
 */
ULONG LDRCALL myLDRQAppType(ULONG p1, ULONG p2)
{
    APIRET rc;

    kprintf(("_LDRQAppType: entry\n"));
    fQAppType = 1;

    rc = LDRQAppType(p1, p2);

    fQAppType = 0;
    kprintf(("_LDRQAppType: exit\n"));

    return rc;
}
