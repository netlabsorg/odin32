/* $Id: KERNEL32.CPP,v 1.42 2000-02-16 14:25:29 sandervl Exp $ */

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

#include "misc.h"
#include <builtin.h>
#include "heap.h"
#include "handlemanager.h"
#include "wprocess.h"
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
#ifdef DEBUG_LOCAL
  dprintf(("KERNEL32:  GetLastError returned %d\n", rc));
#endif
  return(rc);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API SetLastError( DWORD arg1)
{
    dprintf2(("KERNEL32:  SetLastError to %d\n", arg1));
    O32_SetLastError(arg1);
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
BOOL WIN32API GetStringTypeW(DWORD fdwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
{
 int i;

  dprintf(("KERNEL32:  GetStringTypeW, not properly implemented\n"));
  if((DWORD)lpSrcStr == (DWORD)lpCharType || !lpSrcStr || !lpCharType) {
        O32_SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
  }
  if(cchSrc == -1)
        cchSrc = UniStrlen((UniChar*)lpSrcStr);

  memset(lpCharType, 0, cchSrc*sizeof(WORD));
  switch(fdwInfoType) {
        case CT_CTYPE1:
                for(i=0;i<cchSrc;i++) {
                        if(lpSrcStr[i] >= (WCHAR)'a' && lpSrcStr[i] <= (WCHAR)'z')
                                lpCharType[i] |= C1_LOWER | C1_ALPHA;
                        else
                        if(lpSrcStr[i] >= (WCHAR)'A' && lpSrcStr[i] <= (WCHAR)'A')
                                lpCharType[i] |= C1_UPPER | C1_ALPHA;
                        else
                        if(lpSrcStr[i] >= (WCHAR)'0' && lpSrcStr[i] <= (WCHAR)'9')
                                lpCharType[i] |= C1_DIGIT;
                        else
                        if(lpSrcStr[i] >= (WCHAR)' ')
                                lpCharType[i] |= C1_SPACE;
                }
                break;
        case CT_CTYPE2:
        case CT_CTYPE3: //not supported right now
                break;
  }
  return(TRUE);
}
//******************************************************************************
//NOTE: This has one parameter more than the W version! (@#$@#$)
//******************************************************************************
BOOL WIN32API GetStringTypeA(LCID Locale, DWORD fdwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
{
 int i;

  dprintf(("KERNEL32:  GetStringTypeA, not properly implemented\n"));
  if(lpSrcStr == (LPCSTR)lpCharType || !lpSrcStr || !lpCharType) {
        O32_SetLastError(ERROR_INVALID_PARAMETER);
        return(FALSE);
  }
  if(cchSrc == -1)
        cchSrc = strlen(lpSrcStr);

  memset(lpCharType, 0, cchSrc*sizeof(WORD));
  switch(fdwInfoType) {
        case CT_CTYPE1:
                for(i=0;i<cchSrc;i++) {
                        if(lpSrcStr[i] >= 'a' && lpSrcStr[i] <= 'z')
                                lpCharType[i] |= C1_LOWER | C1_ALPHA;
                        else
                        if(lpSrcStr[i] >= 'A' && lpSrcStr[i] <= 'A')
                                lpCharType[i] |= C1_UPPER | C1_ALPHA;
                        else
                        if(lpSrcStr[i] >= '0' && lpSrcStr[i] <= '9')
                                lpCharType[i] |= C1_DIGIT;
                        else
                        if(lpSrcStr[i] >= ' ')
                                lpCharType[i] |= C1_SPACE;
                }
                break;
        case CT_CTYPE2:
        case CT_CTYPE3: //not supported right now
                break;
  }
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetStringTypeExW(LCID Locale, DWORD fdwInfoType, LPCWSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
{
  dprintf(("KERNEL32:  GetStringTypeExW, not properly implemented\n"));
  return(GetStringTypeW(fdwInfoType, lpSrcStr, cchSrc, lpCharType));
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API GetStringTypeExA(LCID Locale, DWORD fdwInfoType, LPCSTR lpSrcStr, int cchSrc, LPWORD lpCharType)
{
  dprintf(("KERNEL32:  GetStringTypeExA, not properly implemented\n"));
  return(GetStringTypeA(Locale, fdwInfoType, lpSrcStr, cchSrc, lpCharType));
}
//******************************************************************************
VOID WIN32API GlobalMemoryStatus( MEMORYSTATUS *arg1)
{
    dprintf(("KERNEL32:  GlobalMemoryStatus\n"));
    O32_GlobalMemoryStatus(arg1);
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
//TODO!
//******************************************************************************
int WIN32API LCMapStringW(
    DWORD /*LCID*/     Locale,
    DWORD    dwMapFlags,
    LPCWSTR  lpSrcStr,
    int      cchSrc,
    LPWSTR   lpDestStr,
    int      cchDest)
{
    // quick hack!  this code is not done!
    if(cchSrc == -1)
        cchSrc = strlen((const char *)lpSrcStr);
    if(!cchDest)
        return cchSrc;
    strncpy((char *)lpDestStr, (const char *)lpSrcStr, max(cchSrc, cchDest));
    return max(cchSrc, cchDest);
}
//******************************************************************************
//TODO!
//******************************************************************************
int WIN32API LCMapStringA(
    DWORD /*LCID*/     Locale,
    DWORD    dwMapFlags,
    LPCSTR   lpSrcStr,
    int      cchSrc,
    LPSTR    lpDestStr,
    int      cchDest)
{
    dprintf(("KERNEL32:  LCMapStringA not implemented\n"));
    if(cchSrc == -1)
        cchSrc = strlen((const char *)lpSrcStr);
    if(!cchDest)
        return cchSrc;
    strncpy((char *)lpDestStr, (const char *)lpSrcStr, max(cchSrc, cchDest));
    return max(cchSrc, cchDest);
}
//******************************************************************************
//SvL: 24-6-'97 - Added
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
UINT WIN32API CompareStringA(LCID lcid, DWORD fdwStyle, LPCSTR lpString1,
                             DWORD cch1, LPCSTR lpString2, DWORD cch2)
{
  int i;
  int fEqual = TRUE;
  char *string1 = (char *)lpString1, *string2 = (char *)lpString2;

#ifdef DEBUG
   if(fdwStyle & SORT_STRINGSORT)
         dprintf(("KERNEL32:  SORT_STRINGSORT not supported!\n"));
   if(fdwStyle & NORM_IGNORENONSPACE)
         dprintf(("KERNEL32:  NORM_IGNORENONSPACE not supported!\n"));
   if(fdwStyle & NORM_IGNORESYMBOLS)
         dprintf(("KERNEL32:  NORM_IGNORESYMBOLS not supported!\n"));
#endif

   if(cch1 == -1) cch1 = strlen(string1);
   if(cch2 == -1) cch2 = strlen(string2);

   if(fdwStyle) {
         //TODO!
         if(fdwStyle != 0 && fdwStyle != NORM_IGNORECASE)
                 return(0);  /*PLF Fri  98-03-13 04:09:32 was return 1 */
   }
   if(fdwStyle & NORM_IGNORECASE)
       fEqual = strnicmp(string1, string2, min(cch1, cch2));
   else
       fEqual = strncmp(string1, string2, min(cch1, cch2));

   if     (fEqual < 0 ) fEqual = 1;
   else if(fEqual == 0) fEqual = 2;
   else if(fEqual >  0) fEqual = 3;

   //If equal, but different length, largest one is the greatest in lexical value
   if(fEqual == 2 && cch1 != cch2){
      if(cch1 < cch2)
         fEqual = 1;
      else
         fEqual = 3;
   }
   dprintf2(("KERNEL32:  OS2CompareStringA '%s' - '%s' returned %d\n", lpString1, lpString2, fEqual));
   return(fEqual);
}

//******************************************************************************
//TODO: Not complete (fdwStyle flags specify compare method)
//******************************************************************************
UINT WIN32API CompareStringW(LCID lcid, DWORD fdwStyle, LPCWSTR lpString1,
                             DWORD cch1, LPCWSTR lpString2, DWORD cch2)
{
 int i;
 int fEqual = TRUE;
 char *string1 = UnicodeToAsciiString((LPWSTR)lpString1);
 char *string2 = UnicodeToAsciiString((LPWSTR)lpString2);

  dprintf2(("KERNEL32:  OS2CompareStringW '%s' - '%s'\n", string1, string2));

  fEqual = CompareStringA(lcid, fdwStyle, string1, cch1, string2, cch2);
  FreeAsciiString(string1);
  FreeAsciiString(string2);

  return(fEqual);
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
//SvL: 26-6-'97 - Added
//******************************************************************************
VOID WIN32API OutputDebugStringW(LPCWSTR arg1)
{
 char *astring;

    astring = UnicodeToAsciiString((LPWSTR)arg1);
    dprintf(("KERNEL32:  OS2OutputDebugStringW %s\n", astring));
    FreeAsciiString(astring);
}
//******************************************************************************
//******************************************************************************
VOID WIN32API OutputDebugStringA(LPCSTR lpszOutputString)
{
  dprintf(("KERNEL32:  OutputDebugStringA: %s\n", lpszOutputString));
  return;
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


int WIN32API GetNumberFormatA(LCID     Locale,
                                 DWORD    dwFlags,
                                 LPCSTR lpValue,
                                 CONST NUMBERFMTA *lpFormat,
                                 LPSTR  lpNumberStr,
                                 int      cchNumber)
{
  dprintf(("KERNEL32::OS2GetNumberFormatA(%08x,%08x,%s,%08x,%s,%08x) not implemented.\n",
           Locale,
           dwFlags,
           lpValue,
           lpFormat,
           lpNumberStr,
           cchNumber));

  return 0;
}

int WIN32API GetNumberFormatW(LCID     Locale,
                                 DWORD    dwFlags,
                                 LPCWSTR lpValue,
                                 CONST NUMBERFMTW *lpFormat,
                                 LPWSTR  lpNumberStr,
                                 int      cchNumber)
{
  dprintf(("KERNEL32::OS2GetNumberFormatW(%08x,%08x,%s,%08x,%s,%08x) not implemented.\n",
           Locale,
           dwFlags,
           lpValue,
           lpFormat,
           lpNumberStr,
           cchNumber));

  return 0;
}

BOOL WIN32API FindCloseChangeNotification(HANDLE hChange)
{
  dprintf(("KERNEL32:  OS2FindNextChangeNotification, Not implemented\n"));

  return(TRUE);
}

//******************************************************************************
//******************************************************************************
void WIN32API WrongComctl32()
{
  O32_MessageBox(NULL,
             "KERNEL32.36 not implemented",
             "Win32 for OS/2 Error",
             MB_OK);
  ExitProcess(987);
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
