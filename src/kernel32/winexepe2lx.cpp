/* $Id: winexepe2lx.cpp,v 1.10 2001-07-08 02:49:47 bird Exp $ */

/*
 * Win32 PE2LX Exe class
 *
 * Copyright 1998-1999 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1999-2000 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSERRORS      /* DOS Error values         */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2wrap.h>        //Odin32 OS/2 api wrappers

#include <stdlib.h>         //getenv

#include <misc.h>
#include <win32type.h>
#include <win32k.h>
#include "winexepe2lx.h"

#include "cio.h"            // I/O
#include "oslibmisc.h"      // OSLibGetDllName
#include "conwin.h"         // Windows Header for console only
#include "console.h"

#define DBG_LOCALLOG    DBG_winexepe2lx
#include "dbglocal.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
BOOL Win32Pe2LxExe::fEarlyInit = FALSE;


/**
 * Register a Pe2Lx Executable module.
 * This is called from the TIBFix code in the Pe2Lx exe. It creates the WinExe object from
 * the instance handle passed in.
 * @param     ulPe2LxVersion   Pe2Lx version number.
 * @param     hinstance        Module handle.
 * @param     ulReserved       Reserved.
 * @sketch    I/O init.
 *            Check that pe2lx version matches the version of kernel32.dll.
 *            Frees WinExe if is not NULL - should never happen!
 *            Write info to the log.
 *            Create Pe2Lx Exe object.
 *            Call start (which calls the entry point).
 * @status    completely implemented.
 * @author    Sander van Leeuwen, knut st. osmundsen
 */
void WIN32API RegisterPe2LxExe(ULONG ulPe2LxVersion, HINSTANCE hinstance, ULONG ulReserved)
{
    Win32Pe2LxExe *pWinPe2LxExe;

    /* I/O init. */
    if (getenv("WIN32_IOPL2"))
        io_init1();

    /* Check that pe2lx version matches the version of kernel32.dll. */
    CheckVersion(ulPe2LxVersion & ~0x80000000UL, OSLibGetDllName(hinstance));

    /* Write info to the log. */
    dprintf(("RegisterPe2LxExe: ulPe2LxVersion = %#x\n", ulPe2LxVersion));
    dprintf(("RegisterPe2LxExe: hinstance = %#x\n", hinstance));
    dprintf(("RegisterPe2LxExe: ulReserved = %#x\n", ulReserved));
    dprintf(("RegisterPe2LxExe: name = %s\n", OSLibGetDllName(hinstance)));

    /* Might allready be initiated because of early init. */
    if (    WinExe != NULL
        &&  (   (ulPe2LxVersion & 0x80000000UL) != 0x80000000UL)
             || !Win32Pe2LxExe::fEarlyInit)
    {
        delete WinExe;
        WinExe = NULL;
    }

    if (WinExe == NULL)
    {
        /* Create Pe2Lx Exe object. */
        pWinPe2LxExe = new Win32Pe2LxExe(hinstance, (ulPe2LxVersion & 0x80000000UL) == 0x80000000UL);
        if (pWinPe2LxExe == NULL)
        {
            eprintf(("RegisterPe2LxExe: new returned a NULL-pointer\n"));
            return;
        }
        if (!pWinPe2LxExe->init())
        {
            eprintf(("RegisterPe2LxExe: init-method failed.\n"));
            delete pWinPe2LxExe;
            return;
        }
    }
    else
        pWinPe2LxExe = (Win32Pe2LxExe*)WinExe;

    /* Call start (which calls the entry point). */
    /*DebugInt3();*/
    pWinPe2LxExe->start();
}


/**
 * Constructor - creates an pe2lx exe object from a module handle to a pe2lx exe module.
 * @param     hinstance   Module handle.
 * @param     fWin32k     TRUE:  Win32k module.
 *                        FALSE: Pe2Lx module.
 * @status    completely implmented.
 * @author    Sander van Leeuwen, knut st. osmundsen
 * @remark    Win32Pe2LxImage may throw an exception!
 */
Win32Pe2LxExe::Win32Pe2LxExe(HINSTANCE hinstance, BOOL fWin32k)
    : Win32ImageBase(hinstance),
    Win32ExeBase(hinstance),
    Win32Pe2LxImage(hinstance, fWin32k)
{
}


/**
 * Destructor - does nothing.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 */
Win32Pe2LxExe::~Win32Pe2LxExe()
{

}


/**
 * Init object.
 * Must be called immedeately after the object construction.
 * @returns   Success indicator. (TRUE == success)
 * @sketch
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
BOOL Win32Pe2LxExe::init()
{
    if (Win32Pe2LxImage::init())
    {
        fConsoleApp = pNtHdrs->OptionalHeader.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI;

        /* console app? */
        if (fConsoleApp)
        {
            APIRET rc;

            dprintf(("Console application!\n"));

            rc = iConsoleInit(TRUE);    /* initialize console subsystem */
            if (rc != NO_ERROR)     /* check for errors */
                dprintf(("KERNEL32:Win32Image:Init ConsoleInit failed with %u.\n", rc));
        }
    }
    else
        return FALSE;
    return TRUE;
}



/**
 * Preinitiate the executable before RegisterPe2LxExe is called.
 * This is done by the first Pe2Lx DLL which is loaded.
 *
 * @returns Success idicator.
 * @status
 * @author  knut st. osmundsen (kosmunds@csc.no)
 * @remark
 */
BOOL Win32Pe2LxExe::earlyInit()
{
    /*
     * Try make an win32k loaded executable object.
     */
    Win32Pe2LxExe * pExe = new Win32Pe2LxExe((HINSTANCE)OSLibGetPIB(PIB_TASKHNDL), libWin32kInstalled());
    if (pExe)
    {
        if (pExe->init())
        {
            WinExe = pExe;
            return fEarlyInit = TRUE;
        }
    }

    return FALSE;
}

