/* $Id: libTest.c,v 1.3.2.1 2002-04-01 13:04:56 bird Exp $
 *
 * Test program for the win32k library.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_BASE

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include <stdio.h>
#include "win32k.h"



int main(void)
{
    APIRET rc;

    rc = libWin32kInit();
    printf("libWin32Init returned %d\n", rc);

    /* this next test is currently disabled! */
    rc = 1;
    if (rc == NO_ERROR)
    {
        PVOID pv = (PVOID)0x80000;
        rc = DosAllocMemEx(&pv, 0x1000, PAG_READ | PAG_WRITE);
        printf("DosAllocMemEx returned %d, pv=0x%08x (was=0x00080000)\n", rc, pv);
    }

    rc = libWin32kTerm();
    printf("libWin32kTerm returned %d\n", rc);
    return rc;
}
