/* $Id: initterm.cpp,v 1.67 2003-01-21 11:21:52 sandervl Exp $
 *
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
#include <options.h>
#include "initterm.h"
#include <win32type.h>
#include <odinlx.h>
#include "oslibmisc.h"
#include <heapshared.h>
#include <heapcode.h>
#include "mmap.h"
#include "directory.h"
#include "hmdevio.h"
#include "windllbase.h"
#include "winexepe2lx.h"
#include <exitlist.h>
#include "oslibdos.h"
#include <cpuhlp.h>
#include <Win32k.h>
#include <initdll.h>
#include <codepage.h>
#include <process.h>

#define DBG_LOCALLOG    DBG_initterm
#include "dbglocal.h"

BOOL  fVersionWarp3 = FALSE;
BOOL  fCustomBuild  = FALSE;

//Global DLL Data
#pragma data_seg(_GLOBALDATA)
int globLoadNr = 0;
#pragma data_seg()

/*-------------------------------------------------------------------*/
/* A clean up routine registered with DosExitList must be used if    */
/* runtime calls are required and the runtime is dynamically linked. */
/* This will guarantee that this clean up routine is run before the  */
/* library DLL is terminated.                                        */
/*-------------------------------------------------------------------*/
static void APIENTRY cleanup(ULONG reason);

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
    size_t i;
    APIRET rc;
    ULONG  ulSysinfo, version[2];

    /*-------------------------------------------------------------------------*/
    /* If ulFlag is zero then the DLL is being loaded so initialization should */
    /* be performed.  If ulFlag is 1 then the DLL is being freed so            */
    /* termination should be performed.                                        */
    /*-------------------------------------------------------------------------*/

    switch (ulFlag)
    {
        case 0:
        {
            if (fInit)
                return 1; // already initialized

            ctordtorInit();

            /*******************************************************************/
            /* A DosExitList routine must be used to clean up if runtime calls */
            /* are required and the runtime is dynamically linked.             */
            /*******************************************************************/

            rc = DosExitList(EXITLIST_KERNEL32|EXLST_ADD, cleanup);
            if (rc)
                return 0;

            rc = DosQuerySysInfo(QSV_VERSION_MAJOR, QSV_VERSION_MINOR,
                                 version, sizeof(version));
            if (rc == 0)
                if(version[0] >= 20 && version[1] <= 30)
                    fVersionWarp3 = TRUE;

            rc = inittermKernel32(hModule, ulFlag);
            break;
        }

        case 1:
            rc = inittermKernel32(hModule, ulFlag);
            break;

        default:
            return 0;
    }

    /***********************************************************/
    /* A non-zero value must be returned to indicate success.  */
    /***********************************************************/
    return rc;
}
//******************************************************************************
//******************************************************************************
static void APIENTRY cleanup(ULONG ulReason)
{
    cleanupKernel32(ulReason);

    ctordtorTerm();

    DosExitList(EXLST_EXIT, cleanup);
    return ;
}
//******************************************************************************
ULONG APIENTRY _O32__DLL_InitTerm(ULONG handle, ULONG flag);
//******************************************************************************
ULONG APIENTRY InitializeKernel32()
{
    HMODULE hModule;

    if (!fInit)
        loadNr = globLoadNr++;

    BOOL WGSS_OK = FALSE;

    if (DosQueryModuleHandleStrict("WGSS50", &hModule) == NO_ERROR)
    {
        if (_O32__DLL_InitTerm(hModule, 0) != 0)
        {
            WGSS_OK = TRUE;

            if (DosQueryModuleHandleStrict("KERNEL32", &hModule) == NO_ERROR)
                return _DLL_InitTerm(hModule, 0);
            else
                ReportFatalDllInitError("KERNEL32");
        }
    }

    if (!WGSS_OK)
        ReportFatalDllInitError("WGSS50");

    return 0; // failure
}
//******************************************************************************
//******************************************************************************
VOID APIENTRY ReportFatalDllInitError(CHAR *pszModName)
{
    static const char msg1[] =
        "Failed to initialize the ";
    static const char msg2[] =
        " library while starting \"";
    static const char msg3[] =
        "\".\n\r"
        "\n\r"
        "It is possible that there is not enough memory in the system to "
        "run this application. Please close other applications and try "
        "again. If the problem persists, please report the details by "
        "creating a ticket at http://svn.netlabs.org/odin32/.\n\r";

    char msg[sizeof(msg1) + 8 + sizeof(msg2) + CCHMAXPATH + sizeof(msg3)];

    strcpy(msg, msg1);
    strncat(msg, pszModName, 8);
    strcat(msg, msg2);

    PPIB ppib;
    DosGetInfoBlocks(NULL, &ppib);
    if (DosQueryModuleName(ppib->pib_hmte, CCHMAXPATH,
                           msg + strlen(msg)) != NO_ERROR)
        strcat(msg, "<unknown executable>");
    strcat(msg, msg3);

    BOOL haveHMQ = FALSE;
    MQINFO mqinfo;
    if (WinQueryQueueInfo(1 /*HMQ_CURRENT*/, &mqinfo, sizeof(mqinfo)) == FALSE)
    {
        // attempt to initialize PM and try again
        HAB hab = WinInitialize(0);
        if (hab)
        {
            HMQ hmq = WinCreateMsgQueue(hab, 0);
            if (hmq)
                haveHMQ = TRUE;
        }
    }
    else
        haveHMQ = TRUE;

    WinMessageBox(HWND_DESKTOP, NULL, msg, "Odin: Fatal Error", 0,
                  MB_APPLMODAL | MB_MOVEABLE | MB_ERROR | MB_OK);

    // duplicate the message to the console just in case (PM may be not
    // available)
    ULONG dummy;
    DosWrite((HFILE)1, (PVOID)&msg, strlen(msg), &dummy);
}

//******************************************************************************
//******************************************************************************

