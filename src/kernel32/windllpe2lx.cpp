/* $Id: windllpe2lx.cpp,v 1.2 1999-10-14 01:37:55 bird Exp $ */

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


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2wrap.h>        //Odin32 OS/2 api wrappers

#include <stdlib.h>

#include <win32type.h>
#include <misc.h>
#include <windllpe2lx.h>
#include <wprocess.h>

#include "cio.h"            // I/O
#include "oslibmisc.h"      // OSLibGetDllName
#include "conwin.h"         // Windows Header for console only
#include "console.h"


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

    //DebugInt3();

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
        try
        {
            pWinMod = new Win32Pe2LxDll(hinstance, (ulPe2LxVersion & 0x80000000UL) == 0x80000000UL);
            if (pWinMod == NULL)
                throw ((ULONG)ERROR_NOT_ENOUGH_MEMORY);

            /* @@@PH 1998/03/17 Console devices initialization */
            iConsoleDevicesRegister();

            /* Add reference and attach dll to process. */
            pWinMod->AddRef();
            pWinMod->attachProcess();
        }
        catch(ULONG ul)
        {
            eprintf(("RegisterPe2LxDLL: Failed to create module object, ul=%d\n", ul));
            DebugInt3();
            return 0;  /* fail dll load */
        }
    }
    else
    {   /* process detach */
        if (pWinMod != NULL && !fFreeLibrary)
            return 0;   /* don't unload (OS/2 dll unload bug) - see OS2.bugs in root dir. */

        #if 0 /* Runtime environment could already be gone, so don't do this */
            dprintf(("KERNEL32: Dll Removed by FreeLibrary or ExitProcess\n"));
        #endif
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
Win32Pe2LxDll::Win32Pe2LxDll(HINSTANCE hinstance, BOOL fWin32k) throw(ULONG)
    : Win32ImageBase(hinstance),
    Win32DllBase(hinstance, NULL),
    Win32Pe2LxImage(hinstance, fWin32k)
{
    dprintf(("Win32Pe2LxDll::Win32Pe2LxDll %s", szModule));
    /* set entry point. */
    dllEntryPoint = (WIN32DLLENTRY)entryPoint;
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
 * Gets pointer to an exported procedure by procedure name.
 * @returns   Address of exported procedure. 0UL if not found.
 * @param     name  Exported procedure name.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 * @remark
 */
ULONG Win32Pe2LxDll::getApi(char *name)
{
    APIRET      rc;
    ULONG       ulApiAddr;

    rc = DosQueryProcAddr(hinstance, 0, name, (PFN *)&ulApiAddr);
    return rc == NO_ERROR ? ulApiAddr : 0;
}


/**
 * Gets pointer to an exported procedure by ordinal.
 * @returns   Pointer to an exported procedure. 0UL if not found.
 * @param     ordinal  Export ordinal number.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 * @remark    FIXME:
 *            This function should be implemented for both Exe and Dll images!
 *            It could be done similar in both peldr image and pe2lx images by
 *            accessing PE structures.
 */
ULONG Win32Pe2LxDll::getApi(int ordinal)
{
    APIRET      rc;
    ULONG       ulApiAddr;

    rc = DosQueryProcAddr(hinstance, ordinal, NULL, (PFN *)&ulApiAddr);

    return rc == NO_ERROR ? ulApiAddr : 0;
}


/**
 * Simple question: -Native LX dll?
 *                  -No!
 * @returns   FALSE.
 * @status    completely implemented.
 * @author    Sander van Leeuwen
 */
BOOL Win32Pe2LxDll::isLxDll()
{
    return FALSE;
}

