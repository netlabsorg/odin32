/* $Id: windllpe2lx.cpp,v 1.10 2001-07-08 02:49:47 bird Exp $ */

/*
 * Win32 PE2LX Dll class
 *
 * Copyright 1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS      /* DOS Error values */
#define INCL_DOSMODULEMGR   /* DOS Module management */
#define INCL_DOSSEMAPHORES

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2wrap.h>        //Odin32 OS/2 api wrappers

#include <stdlib.h>

#include <win32type.h>
#include <misc.h>
#include <windllpe2lx.h>
#include <winexepe2lx.h>
#include <wprocess.h>

#include "cio.h"            // I/O
#include "oslibmisc.h"      // OSLibGetDllName
#include "conwin.h"         // Windows Header for console only
#include "console.h"

#define DBG_LOCALLOG    DBG_windllpe2lx
#include "dbglocal.h"


/**
 * Register an Pe2Lx Dll module. Called from TIBFix code in Dll Pe2Lx module.
 * @returns   1 on success.
 *            0 on failure.
 * @param     Pe2LxVersion  Pe2Lx version. High bit is Win32k indicator and must be masked off!
 * @param     hInstance     Module handle (OS/2).
 * @param     dwAttachType  0 = attach dll
 *                          1 = detach dll
 * @sketch    Try find module.
 *            IF attach process THEN
 *            BEGIN
 *            END
 *                Init I/O.
 *                Get Lib name and match Pe2Lx version with kernel32 version.
 *                Write info to the log.
 *                Try create pe2lx dll object.
 *                Console devices initialization.
 *                Add reference and attach dll to process.
 *            ELSE
 *            BEGIN
 *                IF module found AND not freelibrary THEN
 *                    fail (return 0) due to OS/2 bug.
 *            END
 *            return successfully (return 1)
 * @status    completely implemented.
 * @author    Sander van Leeuwen, knut st. osmundsen
 */
ULONG WIN32API RegisterPe2LxDll(ULONG ulPe2LxVersion, HINSTANCE hinstance, ULONG ulAttachType)
{
    char *pszName;

    #if 1 /* temporary fix */
    if (ulAttachType != 0UL)
        return 0;
    #endif

    Win32Pe2LxDll *pWinMod = (Win32Pe2LxDll *)Win32DllBase::findModule(hinstance);
    if (ulAttachType == 0UL)
    {   /* Process attach */

        /* Init I/O */
        if (getenv("WIN32_IOPL2"))
            io_init1();

        /* Get Lib name and match Pe2Lx version with kernel32 version. */
        pszName = OSLibGetDllName(hinstance);
        CheckVersion(ulPe2LxVersion & ~0x80000000UL, pszName);

        /* Write info to the log. */
        dprintf(("RegisterPe2LxExe: ulPe2LxVersion = %#x\n", ulPe2LxVersion));
        dprintf(("RegisterPe2LxExe: hinstance = %#x\n", hinstance));
        dprintf(("RegisterPe2LxExe: ulAttachType = %#x (reason)\n", ulAttachType));
        dprintf(("RegisterPe2LxExe: name = %s\n", OSLibGetDllName(hinstance)));

        /* Try create pe2lx dll object. */
        pWinMod = new Win32Pe2LxDll(hinstance, (ulPe2LxVersion & 0x80000000UL) == 0x80000000UL);
        if (pWinMod == NULL)
        {
            eprintf(("RegisterPe2LxDll: new returned a NULL-pointer\n"));
            return 0;
        }
        if (!pWinMod->init())
        {
            eprintf(("RegisterPe2LxDll: init-method failed.\n"));
            delete pWinMod;
            return 0;
        }

        /* @@@PH 1998/03/17 Console devices initialization */
        iConsoleDevicesRegister();

        /* Before we attach the DLL we must make sure that we have a valid executable */
        if (!WinExe)
            Win32Pe2LxExe::earlyInit();

        /* Add reference and attach dll to process. */
        pWinMod->AddRef();
        pWinMod->attachProcess();
    }
    else
    {   /* process detach */
        if (pWinMod != NULL)
            return 0;   /* don't unload (OS/2 dll unload bug) - see OS2.bugs in root dir. */
    }

    return 1;   /* success */
}


/**
 * Constructor - creates an pe2lx dll object from a module handle to a pe2lx dll module.
 * @param     hinstance   Module handle.
 * @param     fWin32k     TRUE:  Win32k module.
 *                        FALSE: Pe2Lx module.
 * @status    completely implemented.
 * @author    Sander van Leeuwen, knut st. osmundsen
 */
Win32Pe2LxDll::Win32Pe2LxDll(HINSTANCE hinstance, BOOL fWin32k)
    : Win32ImageBase(hinstance),
    Win32DllBase(hinstance, NULL),
    Win32Pe2LxImage(hinstance, fWin32k)
{
    dprintf(("Win32Pe2LxDll::Win32Pe2LxDll %s", szModule));
}


/**
 * Destructor - does nothing.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 */
Win32Pe2LxDll::~Win32Pe2LxDll()
{
    dprintf(("Win32Pe2LxDll::~Win32Pe2LxDll %s", szModule));
}


/**
 * Init object.
 * Must be called immedeately after objecte construction.
 * @returns   Success indicator. (TRUE == success)
 * @sketch    call init method of the parten class.
 *            set dllEntryPoint
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BOOL Win32Pe2LxDll::init()
{
    if (Win32Pe2LxImage::init())
    {
        /* set entry point. */
        dllEntryPoint = (WIN32DLLENTRY)entryPoint;
    }
    else
        return FALSE;
    return TRUE;
}


/**
 * Simple question: Pe2Lx DLL? Yes!
 * @returns   TRUE.
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BOOL Win32Pe2LxDll::isPe2LxDll() const
{
    return TRUE;
}


/**
 * Simple question: -Native LX dll?
 *                  -No!
 * @returns   FALSE.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 */
BOOL Win32Pe2LxDll::isLxDll() const
{
    return FALSE;
}



