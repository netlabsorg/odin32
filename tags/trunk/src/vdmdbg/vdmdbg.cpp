/* $Id: vdmdbg.cpp,v 1.1 2001-06-13 04:45:33 bird Exp $
 *
 * VDMDbg - Virtual Dos Machine Debug interface.
 * (It's not only debuging. TaskMgr uses it and that's the reason for implementing it.)
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <misc.h>
#include <unicode.h>
#include <versionos2.h>

#include <vdmdbg.h>

#define DBG_LOCALLOG   DBG_vdmdbg
#include "dbglocal.h"

/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
ODINDEBUGCHANNEL(VDMDBG-VDMVDG)

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMProcessException(
    LPDEBUG_EVENT   lpDebugEvent
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x) - stub\n", lpDebugEvent));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetThreadSelectorEntry(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    LPVDMLDT_ENTRY  lpSelectorEntry
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x) - stub\n", hProcess, hThread, wSelector, lpSelectorEntry));
    return FALSE;
}


/**
 *
 * @returns Pointer.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
ULONG  WIN32API VDMGetPointer(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    DWORD           dwOffset,
    BOOL            fProtMode
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x) - stub\n", hProcess, hThread, wSelector, dwOffset, fProtMode));
    return NULL;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Marked as obsolete, use VDMGetContex.
 */
BOOL WIN32API VDMGetThreadContext(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", lpDebugEvent, lpVDMContext));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Marked as obsolete, use VDMSetContex.
 */
BOOL WIN32API VDMSetThreadContext(
    LPDEBUG_EVENT   lpDebugEvent,
    LPVDMCONTEXT    lpVDMContext
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", lpDebugEvent, lpVDMContext));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetContext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPVDMCONTEXT    lpVDMContext
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x) - stub\n", hProcess, hThread, lpVDMContext));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMSetContext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPVDMCONTEXT    lpVDMContext
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x) - stub\n", hProcess, hThread, lpVDMContext));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetSelectorModule(
    HANDLE          hProcess,
    HANDLE          hThread,
    WORD            wSelector,
    PUINT           lpSegmentNumber,
    LPSTR           lpModuleName,
    UINT            nNameSize,
    LPSTR           lpModulePath,
    UINT            nPathSize
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x, %x, %x, %x) - stub\n",
             hProcess, hThread, wSelector, lpSegmentNumber, lpModuleName, nNameSize, lpModulePath, nPathSize));
    return FALSE;
}


/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetModuleSelector(
    HANDLE          hProcess,
    HANDLE          hThread,
    UINT            wSegmentNumber,
    LPSTR           lpModuleName,
    LPWORD          lpSelector
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x) - stub\n",
             hProcess, hThread, wSegmentNumber, lpModuleName, lpSelector));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMModuleFirst(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x) - stub\n",
             hProcess, hThread, lpModuleEntry, lpEventProc, lpData));
    return FALSE;
}


/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMModuleNext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPMODULEENTRY   lpModuleEntry,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x) - stub\n",
             hProcess, hThread, lpModuleEntry, lpEventProc, lpData));
    return FALSE;
}


/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGlobalFirst(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPGLOBALENTRY   lpGlobalEntry,
    WORD            wFlags,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x, %x) - stub\n",
             hProcess, hThread, lpGlobalEntry, wFlags, lpEventProc, lpData));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGlobalNext(
    HANDLE          hProcess,
    HANDLE          hThread,
    LPGLOBALENTRY   lpGlobalEntry,
    WORD            wFlags,
    DEBUGEVENTPROC  lpEventProc,
    LPVOID          lpData
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x, %x) - stub\n",
             hProcess, hThread, lpGlobalEntry, wFlags, lpEventProc, lpData));
    return FALSE;
}



/**
 *
 * @returns Count of Win16 subsystem processes or the count of subsystem processes enumerated before terminating.
 * @param   dwProcessId
 * @param   fp
 * @param   lparam
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
INT WIN32API VDMEnumProcessWOW(
    PROCESSENUMPROC fp,
    LPARAM          lparam
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", fp, lparam));
    return 0;
}


/**
 *
 * @returns Count of Win16 tasks or the count of tasks enumerated before terminating.
 * @param   dwProcessId
 * @param   fp
 * @param   lparam
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
INT WIN32API VDMEnumTaskWOW(
    DWORD           dwProcessId,
    TASKENUMPROC    fp,
    LPARAM          lparam
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x) - stub\n", dwProcessId, fp, lparam));
    return 0;
}


/**
 * Same as VDMEnumTaskWOW, but the callback procedure gets two extra
 * parameters. The module name of the EXE and the full path to the EXE.
 *
 * @returns Count of Win16 tasks or the count of tasks enumerated before terminating.
 * @param   dwProcessId
 * @param   fp
 * @param   lparam
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
INT WIN32API VDMEnumTaskWOWEx(
    DWORD           dwProcessId,
    TASKENUMPROCEX  fp,
    LPARAM          lparam
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x) - stub\n", dwProcessId, fp, lparam));
    return 0;
}


/**
 * This function rudely terminates a WOW task.
 * It's similar to the way TerminateProcess kills a Win32 process.
 * @returns Success indicator.
 * @param   dwProcessId
 * @param   htask
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMTerminateTaskWOW(
    DWORD           dwProcessId,
    WORD            htask
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", dwProcessId, htask));
    return FALSE;
}


/**
 * VDMStartTaskInWOW launches a Win16 task in a pre-existing WOW VDM.
 * The caller is responsible for ensuring  the program is a 16-bit
 * Windows program.  If it is a DOS or Win32 program, it will still
 * be launched from within the target WOW VDM.
 *
 * The supplied command line and show command are passed
 * unchanged to the 16-bit WinExec API in the target WOW VDM.
 *
 * @returns Success indicator.
 * @param   dwProcessId
 * @param   lpCommandLine
 * @param   wShow
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  This routine is ANSI-only.
 *          Not 100% sure this is WIN32API... (kso)
 */
BOOL WIN32API VDMStartTaskInWOW(
    DWORD           dwProcessId,
    LPSTR           lpCommandLine,
    WORD            wShow
)
{
    dprintf(("VDMDBG: " __FUNCTION__ " (%x, %s, %x) - stub\n", dwProcessId, lpCommandLine, wShow));
    return FALSE;
}


/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  SDK says: VDMKillWOW is not implemented.
 */
BOOL WIN32API VDMKillWOW(VOID)
{
    dprintf(("VDMDBG: " __FUNCTION__ " - stub\n"));
    return FALSE;
}


/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  SDK says: VDMDetectWOW is not implemented.
 */
BOOL WIN32API VDMDetectWOW(VOID)
{
    dprintf(("VDMDBG: " __FUNCTION__ " - stub\n"));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @param   hProcess
 * @param   hThread
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMBreakThread(
    HANDLE          hProcess,
    HANDLE          hThread
)
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", hProcess, hThread));
    return FALSE;
}


/**
 *
 * @returns Debug flags.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
DWORD WIN32API VDMGetDbgFlags(
    HANDLE          hProcess
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", hProcess));
    return 0;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMSetDbgFlags(
    HANDLE          hProcess,
    DWORD           dwFlags
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x) - stub\n", hProcess, dwFlags));
    return FALSE;
}

/**
 * Used to check if a module is loaded.
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMIsModuleLoaded(
    LPSTR szPath
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%s) - stub\n", szPath));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetSegmentInfo(
    WORD        Selector,
    ULONG       Offset,
    BOOL        bProtectMode,
    VDM_SEGINFO *pSegInfo
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x) - stub\n", Selector, Offset, bProtectMode, pSegInfo));
    return FALSE;
}

/**
 * This API is used to read the standard .SYM file format.
 * @returns Success indicator.
 * @param   pszModule         module name (max 9 chars)
 * @param   SegNumber        logical segment number of segment (see VDM_SEGINFO)
 * @param   Offset           offset in segment
 * @param   bProtectMode     TRUE for PM, FALSE for V86 mode
 * @param   bNextSymbol      FALSE to find nearest sym BEFORE offset, TRUE for AFTER
 * @param   szSymbolName     receives symbol name (must point to 256 byte buffer)
 * @param   pDisplacement    distance in bytes from nearest symbol
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetSymbol(
    LPSTR   pszModule,
    WORD    SegNumber,
    DWORD   Offset,
    BOOL    bProtectMode,
    BOOL    bNextSymbol,
    LPSTR   szSymbolName,
    PDWORD  pDisplacement
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x, %x, %x) - stub\n",
             pszModule, SegNumber, Offset, bProtectMode, bNextSymbol, szSymbolName, pDisplacement));
    return FALSE;
}

/**
 *
 * @returns Success indicator.
 * @status  stub
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
BOOL WIN32API VDMGetAddrExpression(
    LPSTR  szModule,
    LPSTR  szSymbol,
    LPWORD  Selector,
    PDWORD Offset,
    LPWORD  Type
    )
{
    dprintf(("VDMDBG: " __FUNCTION__ "(%x, %x, %x, %x, %x) - stub\n",
             szModule, szSymbol, Selector, Offset, Type));
    return FALSE;
}
