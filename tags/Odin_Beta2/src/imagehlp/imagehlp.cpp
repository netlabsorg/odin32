/* $Id: imagehlp.cpp,v 1.5 2000-08-02 14:56:26 bird Exp $ */
/*
 *	IMAGEHLP library
 *
 *	Copyright 1998	Patrik Stridvall
 */

#include <os2win.h>
#include <odinwrap.h>
#include <imagehlp.h>
#include <heapstring.h>

ODINDEBUGCHANNEL(imagehlp)

/**********************************************************************/

HANDLE IMAGEHLP_hHeap = (HANDLE) NULL;

static API_VERSION IMAGEHLP_ApiVersion = { 4, 0, 0, 5 };

/***********************************************************************
 *           IMAGEHLP_LibMain (IMAGEHLP.init)
 */
BOOL WINAPI IMAGEHLP_LibMain(
  HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  switch(fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      IMAGEHLP_hHeap = HeapCreate(0, 0x10000, 0);
      break;
    case DLL_PROCESS_DETACH:
      HeapDestroy(IMAGEHLP_hHeap);
      IMAGEHLP_hHeap = (HANDLE) NULL;
      break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    default:
      break;
    }
  return TRUE;
}

/***********************************************************************
 *           ImagehlpApiVersion (IMAGEHLP.@)
 */
PAPI_VERSION WINAPI ImagehlpApiVersion()
{
  return &IMAGEHLP_ApiVersion;
}

/***********************************************************************
 *           ImagehlpApiVersionEx (IMAGEHLP.@)
 */
PAPI_VERSION WINAPI ImagehlpApiVersionEx(PAPI_VERSION AppVersion)
{
  if(!AppVersion)
    return NULL;

  AppVersion->MajorVersion = IMAGEHLP_ApiVersion.MajorVersion;
  AppVersion->MinorVersion = IMAGEHLP_ApiVersion.MinorVersion;
  AppVersion->Revision = IMAGEHLP_ApiVersion.Revision;
  AppVersion->Reserved = IMAGEHLP_ApiVersion.Reserved;

  return AppVersion;
}

/***********************************************************************
 *           MakeSureDirectoryPathExists (IMAGEHLP.@)
 */
BOOL WINAPI MakeSureDirectoryPathExists(LPCSTR DirPath)
{
  dprintf(("stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           MarkImageAsRunFromSwap (IMAGEHLP.@)
 * FIXME
 *   No documentation available.
 */

/***********************************************************************
 *           SearchTreeForFile (IMAGEHLP.@)
 */
BOOL WINAPI SearchTreeForFile(
  LPSTR RootPath, LPSTR InputPathName, LPSTR OutputPathBuffer)
{
  dprintf(("SearchTreeForFile: stub\n"));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           TouchFileTimes (IMAGEHLP.@)
 */
BOOL WINAPI TouchFileTimes(
  HANDLE FileHandle, LPSYSTEMTIME lpSystemTime)
{
  dprintf(("(0x%08x, %p): stub\n",
    FileHandle, lpSystemTime
  ));
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}



