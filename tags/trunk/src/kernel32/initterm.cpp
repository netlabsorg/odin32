/* $Id: initterm.cpp,v 1.5 1999-06-20 10:55:36 sandervl Exp $ */

/*
 * KERNEL32 DLL entry point
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Peter Fitzsimmons
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
#define  INCL_DOSMISC
#define  INCL_DOSPROCESS
#include <os2wrap.h>	//Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include <wprocess.h>
#include <handlemanager.h>

extern "C" {
/*-------------------------------------------------------------------*/
/* _CRT_init is the C run-time environment initialization function.  */
/* It will return 0 to indicate success and -1 to indicate failure.  */
/*-------------------------------------------------------------------*/
int CDECL CRT_init(void);
/*-------------------------------------------------------------------*/
/* _CRT_term is the C run-time environment termination function.     */
/* It only needs to be called when the C run-time functions are      */
/* statically linked.                                                */
/*-------------------------------------------------------------------*/
void CDECL CRT_term(void);
void CDECL _ctordtorInit( void );
void CDECL _ctordtorTerm( void );
}

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);
extern void APIENTRY Win32DllExitList(ULONG reason);

/* Tue 03.03.1998: knut */
/* flag to optimize DosAllocMem to use all the memory on SMP machines */
ULONG flAllocMem;

#ifndef PAG_ANY
    #define PAG_ANY    0x00000400
#endif

#ifndef QSV_VIRTUALADDRESSLIMIT
    #define QSV_VIRTUALADDRESSLIMIT 30
#endif

/****************************************************************************/
/* _DLL_InitTerm is the function that gets called by the operating system   */
/* loader when it loads and frees this DLL for each process that accesses   */
/* this DLL.  However, it only gets called the first time the DLL is loaded */
/* and the last time it is freed for a particular process.  The system      */
/* linkage convention MUST be used because the operating system loader is   */
/* calling this function.                                                   */
/****************************************************************************/
unsigned long SYSTEM _DLL_InitTerm(unsigned long hModule, unsigned long
                                    ulFlag)
{
    size_t i;
    APIRET rc;
    ULONG sysinfo;

    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
        case 0 :

            /*******************************************************************/
            /* The C run-time environment initialization function must be      */
            /* called before any calls to C run-time functions that are not    */
            /* inlined.                                                        */
            /*******************************************************************/

            if (CRT_init() == -1)
                return 0UL;
            _ctordtorInit();

            dprintf(("kernel32 init\n"));
            CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/
            /*******************************************************************/
            /* A DosExitList routine must be used to clean up if runtime calls */
            /* are required and the runtime is dynamically linked.             */
            /*******************************************************************/

            rc = DosExitList(0x0000FF00|EXLST_ADD, cleanup);
            if (rc)
                return 0UL;

            rc = DosExitList(0x00002A00|EXLST_ADD, Win32DllExitList);
            if (rc)
                return 0UL;

            /* knut: check for high memory support */
            rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &sysinfo, sizeof(sysinfo));

            if ( rc == 0 && sysinfo > 512 )   //VirtualAddresslimit is in MB
                   flAllocMem = PAG_ANY;      // high memory support. Let's use it!
            else   flAllocMem = 0;        // no high memory support

	    InitializeTIB(TRUE);
	    //SvL: Do it here instead of during the exe object creation
	    //(std handles can be used in win32 dll initialization routines
	    HMInitialize();             /* store standard handles within HandleManager */
            break;
        case 1 :
            break;
        default  :
            return 0UL;
    }

    /***********************************************************/
    /* A non-zero value must be returned to indicate success.  */
    /***********************************************************/
    return 1UL;
}


static void APIENTRY cleanup(ULONG ulReason)
{
    dprintf(("kernel32 exit\n"));
    _dump_allocated(10);    /*PLF Wed  98-03-18 23:55:07*/
    DestroyTIB();
    _ctordtorTerm();
    CRT_term();
    DosExitList(EXLST_EXIT, cleanup);
    return ;
}
