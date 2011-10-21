/* $Id: initterm.cpp,v 1.20 2001-09-05 10:30:39 bird Exp $
 *
 * WINMM DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 2000 Chris Wohlgemuth
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*-------------------------------------------------------------*/
/* INITERM.C -- Source for a custom dynamic link library       */
/*              initialization and termination (_DLL_InitTerm) */
/*              function.                                      */
/*                                                             */
/* When called to perform initialization, this sample function */
/* gets storage for an array of integers, and initializes its  */
/* elements with random integers.  At termination time, it     */
/* frees the array.  Substitute your own special processing.   */
/*-------------------------------------------------------------*/


/* Include files */
#define  INCL_DOSMODULEMGR
#define  INCL_DOSPROCESS
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <builtin.h>
#include <misc.h>       /*PLF Wed  98-03-18 23:19:26*/
#include <odin.h>
#include <win32type.h>
#include <winconst.h>
#include <odinlx.h>
#include <initdll.h>
#include "auxiliary.h"
#include "winmmtype.h"
#include "waveoutbase.h"
#include <win/options.h>

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
ULONG SYSTEM _DLL_InitTerm(ULONG hModule, ULONG ulFlag)
{
    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
    case 0 :
        ctordtorInit();

        CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
        return inittermWinmm(hModule, ulFlag);
    case 1 :
        inittermWinmm(hModule, ulFlag);
        ctordtorTerm();
        break;
    default  :
        return 0UL;
    }

    /***********************************************************/
    /* A non-zero value must be returned to indicate success.  */
    /***********************************************************/
    return 1UL;
}
//******************************************************************************
//******************************************************************************



