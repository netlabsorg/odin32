/* $Id: KERNEL32.CPP,v 1.54 2000-09-25 19:57:17 phaller Exp $ */

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

#include "misc.h"
#include <builtin.h>
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
  dprintf(("KERNEL32:  IsBadWritePtr: 0x%X size %d rc = %d\n", (int)lpvPtr, cbBytes, rc));
  return(rc);
#else
  return(O32_IsBadReadPtr(lpvPtr, cbBytes));
#endif
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadCodePtr( FARPROC arg1)
{
    dprintf(("KERNEL32:  IsBadCodePtr\n"));
    return O32_IsBadCodePtr(arg1);
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
    dprintf2(("KERNEL32:  GetLastError returned %d\n", rc));
  }
  return(rc);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API SetLastError( DWORD dwError)
{
    if(dwError != 0) {
    	dprintf2(("KERNEL32:  SetLastError to %d\n", dwError));
    }
    O32_SetLastError(dwError);
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetOEMCP(VOID)
{
  dprintf(("KERNEL32:  GetOEMCP\n"));
  return(O32_GetOEMCP());
}
//******************************************************************************
//******************************************************************************
UINT WIN32API GetACP(VOID)
{
  dprintf(("KERNEL32:  GetACP\n"));
  return(O32_GetACP());
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
    lpMemStat->dwTotalPageFile = sys[2];

    // free bytes of paging file
    lpMemStat->dwAvailPageFile = sys[2];

    // user bytes of address space
    lpMemStat->dwTotalVirtual = max(sys[2], sys[3]);
    lpMemStat->dwAvailVirtual = min(sys[2], sys[3]);
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
VOID WIN32API Sleep(DWORD arg1)
{
    dprintf2(("KERNEL32:  Sleep %d\n", arg1));
    O32_Sleep(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API GetPriorityClass(HANDLE arg1)
{
    dprintf(("KERNEL32:  GetPriorityClass\n"));
    return O32_GetPriorityClass(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API SetPriorityClass(HANDLE arg1, DWORD  arg2)
{
    dprintf(("KERNEL32:  SetPriorityClass\n"));
    return O32_SetPriorityClass(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API Beep( DWORD arg1, DWORD  arg2)
{
    dprintf(("KERNEL32:  OS2Beep\n"));
    return O32_Beep(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalAppExitA( UINT arg1, LPCSTR  arg2)
{
    dprintf(("KERNEL32:  OS2FatalAppExitA\n"));
    O32_FatalAppExit(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalAppExitW(UINT arg1, LPCWSTR arg2)
{
 char *astring;

    dprintf(("KERNEL32:  OS2FatalAppExitW\n"));
    astring = UnicodeToAsciiString((LPWSTR)arg2);
    O32_FatalAppExit(arg1, astring);
    //probably won't return, but who cares..
    FreeAsciiString(astring);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API FatalExit( UINT arg1)
{
    dprintf(("KERNEL32:  OS2FatalExit\n"));
    O32_FatalExit(arg1);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadHugeReadPtr( const void * arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  OS2IsBadHugeReadPtr\n"));
    return O32_IsBadHugeReadPtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsBadHugeWritePtr( PVOID arg1, UINT  arg2)
{
    dprintf(("KERNEL32:  OS2IsBadHugeWritePtr\n"));
    return O32_IsBadHugeWritePtr(arg1, arg2);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API IsDBCSLeadByte(BYTE arg1)
{
    dprintf2(("KERNEL32:  OS2IsDBCSLeadByte\n"));
    return O32_IsDBCSLeadByte(arg1);
}
//******************************************************************************
//******************************************************************************
DWORD WIN32API LoadModule( LPCSTR arg1, PVOID  arg2)
{
    dprintf(("KERNEL32:  OS2LoadModule\n"));
    return O32_LoadModule(arg1, arg2);
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
//Borrowed from Wine
//******************************************************************************
ODINPROCEDURE1(GetStartupInfoA,LPSTARTUPINFOA,lpStartupInfo)
{
  lpStartupInfo->cb               = sizeof(STARTUPINFOA);
  lpStartupInfo->lpReserved       = "<Reserved>";
  lpStartupInfo->lpDesktop        = "Desktop";
  lpStartupInfo->lpTitle          = "Title";
  lpStartupInfo->dwX              = 0;
  lpStartupInfo->dwY              = 0;
  lpStartupInfo->dwXSize          = 640;
  lpStartupInfo->dwYSize          = 480;
  lpStartupInfo->dwXCountChars    = 80;      // for console
  lpStartupInfo->dwYCountChars    = 25;
  lpStartupInfo->dwFillAttribute  = 0x0720;
  lpStartupInfo->dwFlags          = STARTF_USESHOWWINDOW   |
                                    STARTF_USEPOSITION     |
                                    STARTF_USESIZE         |
                                    STARTF_USECOUNTCHARS   |
                                    STARTF_USEFILLATTRIBUTE|
                                    STARTF_USESTDHANDLES;
  lpStartupInfo->wShowWindow      = SW_SHOWDEFAULT;
  lpStartupInfo->cbReserved2      = 0;
  lpStartupInfo->lpReserved2      = NULL; /* must be NULL for VC runtime */
  /* @@@PH 98/07/13 Handlemanager support */
  lpStartupInfo->hStdInput        = GetStdHandle(STD_INPUT_HANDLE);
  lpStartupInfo->hStdOutput       = GetStdHandle(STD_OUTPUT_HANDLE);
  lpStartupInfo->hStdError        = GetStdHandle(STD_ERROR_HANDLE);
  return;
}
//******************************************************************************
//Borrowed from Wine
//******************************************************************************
ODINPROCEDURE1(GetStartupInfoW,LPSTARTUPINFOW,lpStartupInfo)
{
  static WCHAR lpReserved[] = {'<', 'R','e','s','e','r','v','e','d','>', 0};
  static WCHAR lpDesktop[]  = {'D', 'e','s','k','t','o','p', 0};
  static WCHAR lpTitle[]    = {'T', 'i','t','l','e', 0};

  // forward call to ascii variant
  GetStartupInfoA((LPSTARTUPINFOA)lpStartupInfo);
  lpStartupInfo->cb          = sizeof(STARTUPINFOW);
  lpStartupInfo->lpReserved  = lpReserved;
  lpStartupInfo->lpDesktop   = lpDesktop;
  lpStartupInfo->lpTitle     = lpTitle;
  return;
}
//******************************************************************************
//TODO: Not complete or correct, but sufficient for now
//******************************************************************************
BOOL WIN32API GetBinaryTypeA(LPCTSTR lpApplicationName, LPDWORD lpBinaryType)
{
  dprintf(("KERNEL32:  OS2GetBinaryTypeA %s\n", lpApplicationName));
  if(strstr(lpApplicationName, ".EXE") ||   strstr(lpApplicationName, ".exe"))
        *lpBinaryType = SCS_32BIT_BINARY;
  else
  if(strstr(lpApplicationName, ".COM") ||   strstr(lpApplicationName, ".com"))
        *lpBinaryType = SCS_DOS_BINARY;
  else
  if(strstr(lpApplicationName, ".PIF") ||   strstr(lpApplicationName, ".pif"))
        *lpBinaryType = SCS_PIF_BINARY;
  else  return(FALSE);
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetBinaryTypeW(LPCWSTR lpApplicationName, LPDWORD lpBinaryType)
{
 BOOL rc;
 char  *astring;

    dprintf(("KERNEL32:  OS2GetBinaryTypeW\n"));
    astring = UnicodeToAsciiString((LPWSTR)lpApplicationName);
    rc = GetBinaryTypeA(astring, lpBinaryType);
    FreeAsciiString(astring);
    return(rc);
}
//******************************************************************************
//TODO: SetLastError
//******************************************************************************
BOOL WIN32API GetVersionExA(OSVERSIONINFOA *lpVersionInformation)
{
   dprintf(("KERNEL32:  OS2GetVersionExA\n"));

   if(lpVersionInformation == NULL || lpVersionInformation->dwOSVersionInfoSize != sizeof(OSVERSIONINFOA))
        return(FALSE);

   lpVersionInformation->dwMajorVersion = ODINNT_MAJOR_VERSION;    //pretend we're NT 4.0
   lpVersionInformation->dwMinorVersion = ODINNT_MINOR_VERSION;
   lpVersionInformation->dwBuildNumber  = ODINNT_BUILD_NR;
   lpVersionInformation->dwPlatformId   = VER_PLATFORM_WIN32_NT;
   strcpy(lpVersionInformation->szCSDVersion, ODINNT_CSDVERSION);
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetVersionExW(OSVERSIONINFOW *lpVersionInformation)
{
   dprintf(("KERNEL32:  OS2GetVersionExW\n"));

   if(lpVersionInformation == NULL || lpVersionInformation->dwOSVersionInfoSize != sizeof(OSVERSIONINFOW))
        return(FALSE);

   lpVersionInformation->dwMajorVersion = ODINNT_MAJOR_VERSION;    //pretend we're NT 4.0
   lpVersionInformation->dwMinorVersion = ODINNT_MINOR_VERSION;
   lpVersionInformation->dwBuildNumber  = ODINNT_BUILD_NR;
   lpVersionInformation->dwPlatformId   = VER_PLATFORM_WIN32_NT;
   lstrcpyW(lpVersionInformation->szCSDVersion, ODINNT_CSDVERSION_W);
   return(TRUE);
}
//******************************************************************************
//******************************************************************************
LONG WIN32API GetVersion()
{
  dprintf(("KERNEL32:  GetVersion\n"));
  // highword 0 = NT, lowword high byte major ver, low byte minor ver
/* @@@PH 98/04/04 MFC30 makes assumptions about process control block        */
/*                structures that lead to crashes if we don't identify as NT */

  return ODINNT_VERSION;
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


//******************************************************************************

/***********************************************************************
*           RtlFillMemory   (KERNEL32.441)
*/
VOID WIN32API RtlFillMemory(LPVOID ptr,
                            UINT len,
                            UINT fill )
{
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32: RtlFillMemory(%08x,%08x,%08x)\n",
           ptr,
           len,
           fill));
#endif

  memset(ptr,
         fill,
         len );
}


/***********************************************************************
*           RtlMoveMemory   (KERNEL32.442)
*/
VOID WIN32API RtlMoveMemory(LPVOID  dst,
                            LPCVOID src,
                            UINT  len )
{
  dprintf2(("KERNEL32: RtlMoveMemory(%08x,%08x,%08x)\n",
           dst,
           src,
           len));

  memmove(dst,
          src,
          len );
}


/***********************************************************************
*           RtlZeroMemory   (KERNEL32.444)
*/
VOID WIN32API RtlZeroMemory(LPVOID ptr,
                            UINT len)
{
  dprintf2(("KERNEL32: RtlZeroMemory(%08x,%08x)\n",
           ptr,
           len));

  memset(ptr,
         0,
         len);
}

//******************************************************************************
/*KSO Thu 21.05.1998*/
BOOL WIN32API IsDBCSLeadByteEx(UINT CodePage, BYTE TestChar)
{
    dprintf(("KERNEL32:  OS2IsDBCSLeadByteEx - not correctly implemented\n"));
    return O32_IsDBCSLeadByte(TestChar);
}
//******************************************************************************








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
  lpSystemPowerStatus->ACLineStatus       = 1; //online
  lpSystemPowerStatus->BatteryFlag        = 128; //no system battery
  lpSystemPowerStatus->BatteryLifePercent = 255; //unknown
  lpSystemPowerStatus->reserved           = 0;
  lpSystemPowerStatus->BatteryLifeTime    = -1; //unknown
  lpSystemPowerStatus->BatteryFullLifeTime= -1; //unknown

  return TRUE;
}
