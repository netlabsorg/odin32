/* $Id: initterm.cpp,v 1.26 1999-11-22 20:35:50 sandervl Exp $ */

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
#define  INCL_DOSSEMAPHORES
#include <os2wrap.h>    //Odin32 OS/2 api wrappers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <misc.h>
#include <wprocess.h>
#include "handlemanager.h"
#include "profile.h"
#include "initterm.h"
#include <win32type.h>
#include <odinlx.h>
#include "oslibmisc.h"
#include <heapshared.h>
#include "mmap.h"
#include "directory.h"
#include "hmdevio.h"
#include <windllbase.h>

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);
extern void APIENTRY Win32DllExitList(ULONG reason);

extern "C" {
void CDECL _ctordtorInit( void );
void CDECL _ctordtorTerm( void );
}

//Global DLL Data
#pragma data_seg(_GLOBALDATA)
int globLoadNr = 0;
#pragma data_seg()

/* Tue 03.03.1998: knut */
ULONG flAllocMem = 0;    /* flag to optimize DosAllocMem to use all the memory on SMP machines */
ULONG ulMaxAddr = 0x20000000; /* end of user address space. */
int   loadNr = 0;
char  kernel32Path[CCHMAXPATH] = "";

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
    ULONG  ulSysinfo;

    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
        case 0 :
	{
            loadNr = globLoadNr++;

	    strcpy(kernel32Path, OSLibGetDllName(hModule));
	    char *endofpath = strrchr(kernel32Path, '\\');
	    *(endofpath+1) = 0;
            dprintf(("kernel32 init\n"));
            _ctordtorInit();

            CheckVersionFromHMOD(PE2LX_VERSION, hModule); /*PLF Wed  98-03-18 05:28:48*/

            if(InitializeSharedHeap() == FALSE)
                return 0UL;

            PROFILE_LoadOdinIni();
            if(RegisterLxDll(hModule, 0, 0) == FALSE)
                return 0UL;

            /*******************************************************************/
            /* A DosExitList routine must be used to clean up if runtime calls */
            /* are required and the runtime is dynamically linked.             */
            /*******************************************************************/

            rc = DosExitList(0x0000F000|EXLST_ADD, cleanup);
            if (rc)
                return 0UL;

            /* knut: check for high memory support */
            rc = DosQuerySysInfo(QSV_VIRTUALADDRESSLIMIT, QSV_VIRTUALADDRESSLIMIT, &ulSysinfo, sizeof(ulSysinfo));
            if (rc == 0 && ulSysinfo > 512)   //VirtualAddresslimit is in MB
            {
                flAllocMem = PAG_ANY;      // high memory support. Let's use it!
                ulMaxAddr = ulSysinfo * (1024*1024);
            }
            else
                flAllocMem = 0;        // no high memory support

            InitializeTIB(TRUE);
            //SvL: Do it here instead of during the exe object creation
            //(std handles can be used in win32 dll initialization routines
            HMInitialize();             /* store standard handles within HandleManager */
            InitDirectories();
            RegisterDevices();
	    Win32DllBase::setDefaultRenaming();
            break;
	}
        case 1 :
            UnregisterLxDll(hModule);
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
    dprintf(("kernel32 exit %d\n", ulReason));
    //Flush and delete all open memory mapped files
    Win32MemMap::deleteAll();

    WriteOutProfiles();
    DestroyTIB();
    DestroySharedHeap();
    _ctordtorTerm();

    //NOTE: Must be done after DestroyTIB
    CloseLogFile();

    DosExitList(EXLST_EXIT, cleanup);
    return ;
}
