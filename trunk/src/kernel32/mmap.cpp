/* $Id: mmap.cpp,v 1.2 1999-05-31 22:08:09 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include "unicode.h"
#include "mmap.h"
#include "misc.h"


//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateFileMappingA(HANDLE hFile, LPSECURITY_ATTRIBUTES lpsa,
                      DWORD fdwProtect, DWORD dwMaximumSizeHigh,
                      DWORD dwMaximumSizeLow, LPCTSTR lpszMapName)
{
  dprintf(("CreateFileMappingA Not Implemented!\n"));
  dprintf(("CreateFileMappingA handle %d, dwMaximumSize %X%X!\n", hFile, dwMaximumSizeHigh, dwMaximumSizeLow));
  if(lpszMapName){
    dprintf(("lpszMapName %s\n", lpszMapName));
  }
  return(0);
}
//******************************************************************************
//******************************************************************************
HANDLE WIN32API CreateFileMappingW(HANDLE hFile, LPSECURITY_ATTRIBUTES lpsa,
                      DWORD fdwProtect, DWORD dwMaximumSizeHigh,
                      DWORD dwMaximumSizeLow, LPCWSTR lpszMapName)
{
  dprintf(("CreateFileMappingW Not Implemented!\n"));
  return(0);
}
//******************************************************************************
//******************************************************************************
LPVOID WIN32API MapViewOfFileEx(HANDLE hMapObject, DWORD fdwAccess,
                   DWORD dwOffsetHigh, DWORD dwOffsetLow,
                   DWORD cbMap, LPVOID lpvBase)
{
  dprintf(("MapViewOfFileEx Not Implemented!\n"));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API UnmapViewOfFile(LPVOID lpBaseAddress)
{
  dprintf(("UnmapViewOfFile Not Implemented! - TRUE\n"));
  return(TRUE);
}
//******************************************************************************
//******************************************************************************
LPVOID WIN32API MapViewOfFile(HANDLE hMapObject, DWORD fdwAccess,
                 DWORD dwOffsetHigh, DWORD dwOffsetLow,
                 DWORD cbMap)
{
  dprintf(("MapViewOfFile Not Implemented!\n"));
  return(NULL);
}
//******************************************************************************
//******************************************************************************
