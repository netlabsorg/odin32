/* $Id: yield.c,v 1.3 1999-10-31 23:57:08 bird Exp $
 *
 * Yield - conversion may take some time. So it is necessary to
 *         check it's time to yield the processor to other processes.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include "dev32.h"
#include "dev32hlp.h"
#include "yield.h"


/**
 * Checks if we have to yield the CPU. Yield it if we must.
 */
BOOL Yield(void)
{
    PBYTE  pfbYield;

    pfbYield = D32Hlp_GetDOSVar(DHGETDOSV_YIELDFLAG, 0UL);
    if (pfbYield != NULL && *pfbYield)
    {
        D32Hlp_Yield();
        return TRUE;
    }

    return FALSE;
}
