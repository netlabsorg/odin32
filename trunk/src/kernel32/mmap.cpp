/* $Id: mmap.cpp,v 1.5 1999-06-17 22:06:10 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include "unicode.h"
#include "mmap.h"

// REMARK: HANDLEMANAGER SUPPORT TO INTEGRATE ! (PH)


/*****************************************************************************
 * Name      : HANDLE WIN32API CreateFileMappingA
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateFileMappingA(HANDLE                hFile,
                                   LPSECURITY_ATTRIBUTES lpsa,
                                   DWORD                 fdwProtect,
                                   DWORD                 dwMaximumSizeHigh,
                                   DWORD                 dwMaximumSizeLow,
                                   LPCTSTR               lpszMapName)
{
  dprintf(("KERNEL32: CreateFileMappingA(%08xh,%08xh,%08xh,%08xh,%s) not implemented.\n",
           hFile,
           lpsa,
           fdwProtect,
           dwMaximumSizeHigh,
           dwMaximumSizeLow,
           lpszMapName));

  return(0);
}


/*****************************************************************************
 * Name      : HANDLE WIN32API CreateFileMappingW
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

HANDLE WIN32API CreateFileMappingW(HANDLE                hFile,
                                   LPSECURITY_ATTRIBUTES lpsa,
                                   DWORD                 fdwProtect,
                                   DWORD                 dwMaximumSizeHigh,
                                   DWORD                 dwMaximumSizeLow,
                                   LPCWSTR               lpszMapName)
{
  dprintf(("KERNEL32: CreateFileMappingW(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hFile,
           lpsa,
           fdwProtect,
           dwMaximumSizeHigh,
           dwMaximumSizeLow,
           lpszMapName));

  return(0);
}


/*****************************************************************************
 * Name      : LPVOID WIN32API MapViewOfFileEx
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

LPVOID WIN32API MapViewOfFileEx(HANDLE hMapObject,
                                DWORD fdwAccess,
                                DWORD dwOffsetHigh,
                                DWORD dwOffsetLow,
                                DWORD cbMap,
                                LPVOID lpvBase)
{
  dprintf(("KERNEL32: MapViewOfFileEx(%08xh,%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hMapObject,
           fdwAccess,
           dwOffsetHigh,
           dwOffsetLow,
           cbMap,
           lpvBase));

  return(NULL);
}


/*****************************************************************************
 * Name      : BOOL WIN32API UnmapViewOfFileEx
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

BOOL WIN32API UnmapViewOfFile(LPVOID lpBaseAddress)
{
  dprintf(("KERNEL32: UnmapViewOfFile(%08xh) not impemented.\n",
           lpBaseAddress));

  return(TRUE);
}


/*****************************************************************************
 * Name      : BOOL WIN32API MapViewOfFile
 * Purpose   : forward call to Open32
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Fri, 1998/06/12 03:44]
 *****************************************************************************/

LPVOID WIN32API MapViewOfFile(HANDLE hMapObject,
                              DWORD  fdwAccess,
                              DWORD  dwOffsetHigh,
                              DWORD  dwOffsetLow,
                              DWORD  cbMap)
{
  dprintf(("KERNEL32: MapViewOfFile(%08xh,%08xh,%08xh,%08xh,%08xh) not implemented.\n",
           hMapObject,
           fdwAccess,
           dwOffsetHigh,
           dwOffsetLow,
           cbMap));

  return(NULL);
}
