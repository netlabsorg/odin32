/* $Id: KERNEL32.CPP,v 1.75 2003-04-29 10:27:18 sandervl Exp $ */

/*
 * Win32 compatibility file functions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 * Copyright 1998 Peter Fitzsimmons
 * Copyright 1998 Knut St. Osmundsen
 *
 * @(#) KERNEL32.CPP            1.0.1   1998/06/12 PH added HandleManager support
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

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
#include "heap.h"
#include "handlemanager.h"
#include "wprocess.h"
#include "oslibdos.h"
#include <versionos2.h>

#define DBG_LOCALLOG	DBG_kernel32
#include "dbglocal.h"

/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

ODINDEBUGCHANNEL(KERNEL32-KERNEL32)

                /* this define enables certain less important debug messages */
//#define DEBUG_LOCAL 1



/*****************************************************************************
 * Name      : DWORD GetHandleInformation
 * Purpose   : The GetHandleInformation function obtains information about certain
 *             properties of an object handle. The information is obtained as a set of bit flags.
 * Parameters: HANDLE  hObject
 *             LPDWORD lpdwFlags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/

BOOL WIN32API GetHandleInformation(HANDLE  hObject,
                                   LPDWORD lpdwFlags)
{
   return HMGetHandleInformation(hObject, lpdwFlags);
}

/*****************************************************************************
 * Name      : BOOL SetHandleInformation
 * Purpose   : The SetHandleInformation function sets certain properties of an
 *             object handle. The information is specified as a set of bit flags.
 * Parameters: HANDLE hObject  handle to an object
 *             DWORD  dwMask   specifies flags to change
 *             DWORD  dwFlags  specifies new values for flags
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : 
 *
 * Author    : SvL
 *****************************************************************************/

BOOL WIN32API SetHandleInformation(HANDLE hObject,
                                   DWORD  dwMask,
                                   DWORD  dwFlags)
{
   return HMSetHandleInformation(hObject, dwMask, dwFlags);
}
/*****************************************************************************
 * Name      : BOOL WIN32API CloseHandle
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/
BOOL WIN32API CloseHandle(HANDLE hHandle)
{
  dprintf(("KERNEL32: CloseHandle(%08xh)\n",
           hHandle));

  return HMCloseHandle(hHandle);
}


//******************************************************************************
HANDLE WIN32API GetStdHandle(DWORD fdwDevice)
{
 HANDLE handle;

  /* @@@PH 1998/02/12 Handle Manager Support */
  handle = HMGetStdHandle(fdwDevice);

  //@@@PH translate handle

  /* handle = GetStdHandle(fdwDevice); */
  dprintf(("KERNEL32:  GetStdHandle for device %X returned %X\n", fdwDevice, handle));
  return(handle);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetStdHandle(DWORD  IDStdHandle,
                              HANDLE hHandle)
{
  dprintf(("KERNEL32:  SetStdHandle\n"));

  ///@@@PH translate handle

  return (HMSetStdHandle(IDStdHandle,
                         hHandle));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadWritePtr(LPVOID lpvPtr, UINT cbBytes)
{
#ifdef DEBUG
 BOOL rc;

  rc = O32_IsBadWritePtr(lpvPtr, cbBytes);
  dprintf(("KERNEL32:  IsBadWritePtr: 0x%X size %d rc = %d\n", (int)lpvPtr, cbBytes, rc));
  return(rc);
#else
  return(O32_IsBadWritePtr(lpvPtr, cbBytes));
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadReadPtr(CONST VOID *lpvPtr, UINT cbBytes)
{
#ifdef DEBUG
 BOOL rc;

  rc = O32_IsBadReadPtr(lpvPtr, cbBytes);
  dprintf(("KERNEL32:  IsBadReadPtr: 0x%X size %d rc = %d\n", (int)lpvPtr, cbBytes, rc));
  return(rc);
#else
  return(O32_IsBadReadPtr(lpvPtr, cbBytes));
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadCodePtr(FARPROC pCode)
{
    dprintf(("KERNEL32: IsBadCodePtr %x", pCode));
    return O32_IsBadCodePtr(pCode);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadStringPtrA( LPCSTR arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  IsBadStringPtr"));
    return O32_IsBadStringPtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadStringPtrW(LPCWSTR arg1, UINT arg2)
{
    dprintf(("KERNEL32:  OS2IsBadStringPtrW"));
    return O32_IsBadReadPtr((CONST VOID *)arg1, arg2*2+2);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetLastError()
{
 DWORD rc;

  rc = O32_GetLastError();
  if(rc) {
       dprintf(("KERNEL32: GetLastError returned %d\n", rc));
  }
  else dprintf2(("KERNEL32: GetLastError returned no error\n", rc));
  return(rc);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API SetLastError( DWORD dwError)
{
    if(dwError != 0) {
    	dprintf(("KERNEL32:  SetLastError to %d\n", dwError));
    }
    O32_SetLastError(dwError);
}
//******************************************************************************
//******************************************************************************
/*
 * PH 2000/09/25 This is an experiment to overcome some problems
 * with Open32's GMS variant.
 */
void _GlobalMemoryStatus(MEMORYSTATUS *lpMemStat)
{
  ULONG  sys[5];
  // Note: QSV_TOTPHYSMEM = 17, QSV_MAXSHMEM = 21
  lpMemStat->dwLength = sizeof(MEMORYSTATUS);
  
  if(!OSLibDosQuerySysInfo( 17, 21, (PVOID)sys, sizeof(sys)))
  {
    // Specified a number between 0 and 100 that gives a general idea of
    // current memory utilization, in which 0 indicates no memory use and
    // 100 indicates full memory use

    //#define MB512 0x1c000000
    //lpMemStat->dwMemoryLoad = (MB512-sys[20]) * 100 / MB512;
    lpMemStat->dwMemoryLoad = (sys[1] * 100) / sys[0];
    
    // bytes of physical memory
    lpMemStat->dwTotalPhys = sys[0];
      
    // free physical memory bytes
    lpMemStat->dwAvailPhys = sys[0] - sys[1];

    // bytes of paging file
    // @@@PH add swapper.dat size?
    // SvL: Some stupid apps interpret this as a signed long
    lpMemStat->dwTotalPageFile = (sys[2] > 0x80000000) ? 0x7fffffff : sys[2];

    // free bytes of paging file
    lpMemStat->dwAvailPageFile = (sys[2] > 0x80000000) ? 0x7fffffff : sys[2];

    // user bytes of address space
    lpMemStat->dwTotalVirtual = max(lpMemStat->dwTotalPageFile, sys[3]);
    lpMemStat->dwAvailVirtual = min(lpMemStat->dwAvailPageFile, sys[3]);
  }
}



VOID WIN32API GlobalMemoryStatus(MEMORYSTATUS *arg1)
{
    dprintf(("KERNEL32:  GlobalMemoryStatus\n"));
    //O32_GlobalMemoryStatus(arg1);
    _GlobalMemoryStatus(arg1);
    dprintf(("dwMemoryLoad    %X\n", arg1->dwMemoryLoad));
    dprintf(("dwTotalPhys     %X\n", arg1->dwTotalPhys));
    dprintf(("dwAvailPhys     %X\n", arg1->dwAvailPhys));
    dprintf(("dwTotalPageFile %X\n", arg1->dwTotalPageFile));
    dprintf(("dwAvailPageFile %X\n", arg1->dwAvailPageFile));
    dprintf(("dwTotalVirtual  %X\n", arg1->dwTotalVirtual));
    dprintf(("dwAvailVirtual  %X\n", arg1->dwAvailVirtual));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Beep( DWORD arg1, DWORD  arg2)
{
    dprintf(("KERNEL32: Beep %d %d", arg1, arg2));
    return OSLibDosBeep(arg1, arg2);
}
//******************************************************************************
//******************************************************************************

typedef INT (WINAPI *MessageBoxA_funcptr)(HWND,LPCSTR,LPCSTR,UINT);
typedef INT (WINAPI *MessageBoxW_funcptr)(HWND,LPCWSTR,LPCWSTR,UINT);

//******************************************************************************
//******************************************************************************
VOID WIN32API FatalAppExitA( UINT exitCode, LPCSTR  str)
{
    HMODULE mod = GetModuleHandleA( "user32.dll" );
    MessageBoxA_funcptr pMessageBoxA = NULL;

    dprintf(("KERNEL32: FatalAppExitA %d %s", exitCode, str));

    if (mod) pMessageBoxA = (MessageBoxA_funcptr)GetProcAddress( mod, "MessageBoxA" );
    if(pMessageBoxA) pMessageBoxA( 0, str, NULL, MB_SYSTEMMODAL | MB_OK );

    ExitProcess(exitCode);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalAppExitW(UINT exitCode, LPCWSTR str)
{
    HMODULE mod = GetModuleHandleA( "user32.dll" );
    MessageBoxW_funcptr pMessageBoxW = NULL;

    dprintf(("KERNEL32: FatalAppExitW %d %ls", exitCode, str));

    if (mod) pMessageBoxW = (MessageBoxW_funcptr)GetProcAddress( mod, "MessageBoxW" );
    if(pMessageBoxW) pMessageBoxW( 0, str, NULL, MB_SYSTEMMODAL | MB_OK );

    ExitProcess(exitCode);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalExit( UINT exitCode)
{
    dprintf(("KERNEL32: FatalExit %x", exitCode));
    ExitProcess(exitCode);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadHugeReadPtr( const void * arg1, UINT  arg2)
{
    return IsBadReadPtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadHugeWritePtr( PVOID arg1, UINT  arg2)
{
    return IsBadWritePtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
int WIN32API MulDiv(int arg1, int arg2, int  arg3)
{
    dprintf2(("KERNEL32:  MulDiv %d*%d/%d\n", arg1, arg2, arg3));
    if(arg3 == 0)
	return 0;

    return O32_MulDiv(arg1, arg2, arg3);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API FlushInstructionCache(     /*PLF Mon  98-02-09 23:56:49 : STUB STUB STUB STUB STUB */
    HANDLE hProcess,    /* process with cache to flush  */
    LPCVOID lpvBase,    /* address of region to flush   */
    DWORD cbFlush)      /* length of region to flush    */

{
        dprintf(("FlushInstructionCache() - NIY\n"));
        return TRUE;
}


/*****************************************************************************
 * Name      : BOOL GetSystemPowerStatus
 * Purpose   : The GetSystemPowerStatus function retrieves the power status of
 *             the system. The status indicates whether the system is running
 *             on AC or DC power, whether the battery is currently charging,
 *             and how much battery life currently remains.
 * Parameters: LPSYSTEM_POWER_STATUS lpSystemPowerStatus
 * Variables :
 * Result    : TRUE / FALSE
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Mon, 1998/06/15 08:00]
 *****************************************************************************/

BOOL WIN32API GetSystemPowerStatus(LPSYSTEM_POWER_STATUS lpSystemPowerStatus)
{
  dprintf(("Kernel32: GetSystemPowerStatus(%08xh) not properly implemented.\n",
           lpSystemPowerStatus));

  if(lpSystemPowerStatus == NULL) {
	SetLastError(ERROR_INVALID_PARAMETER);
	return FALSE;
  }
  lpSystemPowerStatus->ACLineStatus       = AC_LINE_ONLINE;
  lpSystemPowerStatus->BatteryFlag        = BATTERY_FLAG_HIGH;
  lpSystemPowerStatus->BatteryLifePercent = BATTERY_PERCENTAGE_UNKNOWN;
  lpSystemPowerStatus->Reserved1          = 0;
  lpSystemPowerStatus->BatteryLifeTime    = BATTERY_LIFE_UNKNOWN;
  lpSystemPowerStatus->BatteryFullLifeTime= BATTERY_LIFE_UNKNOWN;

  return TRUE;
}
