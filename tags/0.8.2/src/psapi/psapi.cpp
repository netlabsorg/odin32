/* $Id: psapi.cpp,v 1.3 1999-11-12 11:38:44 sandervl Exp $ */
/*
 *      PSAPI library
 *
 *      Copyright 1998  Patrik Stridvall
 * 	Copyright 1999  Jens Wiessner
 */

#include <os2win.h>
#include <odinwrap.h>
#include <winerror.h>
#include <psapi.h>
#include <string.h>

ODINDEBUGCHANNEL(psapi)

/***********************************************************************
 *           EmptyWorkingSet (PSAPI.1)
 */
BOOL WINAPI EmptyWorkingSet(HANDLE hProcess)
{
#ifdef DEBUG
  dprintf(("PSAPI: EmptyWorkingSet\n"));
#endif
  return SetProcessWorkingSetSize(hProcess, 0xFFFFFFFF, 0xFFFFFFFF);
}

/***********************************************************************
 *           EnumDeviceDrivers (PSAPI.2)
 */
BOOL WINAPI EnumDeviceDrivers(
  LPVOID *lpImageBase, DWORD cb, LPDWORD lpcbNeeded)
{
#ifdef DEBUG
  dprintf(("PSAPI: EnumDeviceDrivers not implemented\n"));
#endif
  if(lpcbNeeded)
    *lpcbNeeded = 0;

  return TRUE;
}    


/***********************************************************************
 *           EnumProcesses (PSAPI.3)
 */
BOOL WINAPI EnumProcesses(DWORD *lpidProcess, DWORD cb, DWORD *lpcbNeeded)
{
#ifdef DEBUG
  dprintf(("PSAPI: EnumProcesses not implemented\n"));
#endif
  if(lpcbNeeded)
    *lpcbNeeded = 0;

  return TRUE;
}

#ifndef __WIN32OS2__
/***********************************************************************
 *           EnumProcessModules (PSAPI.4)
 */
BOOL WINAPI EnumProcessModules(
  HANDLE hProcess, HMODULE *lphModule, DWORD cb, LPDWORD lpcbNeeded)
{
#ifdef DEBUG
  dprintf(("PSAPI: EnumProcessModules not implemented\n"));
#endif
  if(lpcbNeeded)
    *lpcbNeeded = 0;

  return TRUE;
}
#endif

/***********************************************************************
 *          GetDeviceDriverBaseNameA (PSAPI.5)
 */
DWORD WINAPI GetDeviceDriverBaseNameA(
  LPVOID ImageBase, LPSTR lpBaseName, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetDeviceDrivers not implemented\n"));
#endif
  if(lpBaseName && nSize)
    lpBaseName[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetDeviceDriverBaseNameW (PSAPI.6)
 */
DWORD WINAPI GetDeviceDriverBaseNameW(
  LPVOID ImageBase, LPWSTR lpBaseName, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetDeviceDriverBaseNameW not implemented\n"));
#endif
  if(lpBaseName && nSize)
    lpBaseName[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetDeviceDriverFileNameA (PSAPI.7)
 */
DWORD WINAPI GetDeviceDriverFileNameA(
  LPVOID ImageBase, LPSTR lpFilename, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetDeviceDriverFileNameA not implemented\n"));
#endif
  if(lpFilename && nSize)
    lpFilename[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetDeviceDriverFileNameW (PSAPI.8)
 */
DWORD WINAPI GetDeviceDriverFileNameW(
  LPVOID ImageBase, LPWSTR lpFilename, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetDeviceDriverFileNameW not implemented\n"));
#endif
  if(lpFilename && nSize)
    lpFilename[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetMappedFileNameA (PSAPI.9)
 */
DWORD WINAPI GetMappedFileNameA(
  HANDLE hProcess, LPVOID lpv, LPSTR lpFilename, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetMappedFileNameA not implemented\n"));
#endif
  if(lpFilename && nSize)
    lpFilename[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetMappedFileNameW (PSAPI.10)
 */
DWORD WINAPI GetMappedFileNameW(
  HANDLE hProcess, LPVOID lpv, LPWSTR lpFilename, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetMappedFileNameW not implemented\n"));
#endif
  if(lpFilename && nSize)
    lpFilename[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetModuleBaseNameA (PSAPI.11)
 */
DWORD WINAPI GetModuleBaseNameA(
  HANDLE hProcess, HMODULE hModule, LPSTR lpBaseName, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetModuleBaseNameA not implemented\n"));
#endif
  if(lpBaseName && nSize)
    lpBaseName[0] = '\0';

  return 0;
}

/***********************************************************************
 *           GetModuleBaseNameW (PSAPI.12)
 */
DWORD WINAPI GetModuleBaseNameW(
  HANDLE hProcess, HMODULE hModule, LPWSTR lpBaseName, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetModuleBaseNameW not implemented\n"));
#endif
  if(lpBaseName && nSize)
    lpBaseName[0] = '\0';

  return 0;
}

#ifdef __WIN32OS2__
/***********************************************************************
 *           GetModuleFileNameExA (PSAPI.13)
 */
DWORD WINAPI GetModuleFileNameExA(
  HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
   if(hProcess != GetCurrentProcess()) {
       dprintf(("PSAPI: GetModuleFileNameExA not implemented for other processes (%x %x)\n", hProcess, hModule));
       return 0;
   }
   return GetModuleFileNameA(hModule, lpFilename, nSize);
}
#else
/***********************************************************************
 *           GetModuleFileNameExA (PSAPI.13)
 */
DWORD WINAPI GetModuleFileNameExA(
  HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
  if(lpFilename&&nSize)
    lpFilename[0]='\0';

  return 0;
}
#endif

/***********************************************************************
 *           GetModuleFileNameExW (PSAPI.14)
 */
DWORD WINAPI GetModuleFileNameExW(
  HANDLE hProcess, HMODULE hModule, LPWSTR lpFilename, DWORD nSize)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetModuleFileNameExW not implemented\n"));
#endif
  if(lpFilename && nSize)
    lpFilename[0] = '\0';

  return 0;
}

#ifndef __WIN32OS2__
/***********************************************************************
 *           GetModuleInformation (PSAPI.15)
 */
BOOL WINAPI GetModuleInformation(
  HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetModuleInformation not implemented\n"));
#endif
  memset(lpmodinfo, 0, cb);

  return TRUE;
}
#endif

/***********************************************************************
 *           GetProcessMemoryInfo (PSAPI.16)
 */
BOOL WINAPI GetProcessMemoryInfo(
  HANDLE Process, PPROCESS_MEMORY_COUNTERS ppsmemCounters, DWORD cb)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetProcessMemoryInfo not implemented\n"));
#endif
  memset(ppsmemCounters, 0, cb);

  return TRUE;
}

/***********************************************************************
 *           GetWsChanges (PSAPI.17)
 */
BOOL WINAPI GetWsChanges(
  HANDLE hProcess, PPSAPI_WS_WATCH_INFORMATION lpWatchInfo, DWORD cb)
{
#ifdef DEBUG
  dprintf(("PSAPI: GetWsChanges not implemented\n"));
#endif
  memset(lpWatchInfo, 0, cb);

  return TRUE;
}

/***********************************************************************
 *           InitializeProcessForWsWatch (PSAPI.18)
 */
BOOL WINAPI InitializeProcessForWsWatch(HANDLE hProcess)
{
#ifdef DEBUG
  dprintf(("PSAPI: InitializeProcessForWsWatch not implemented\n"));
#endif
  return TRUE;
}

/***********************************************************************
 *           QueryWorkingSet (PSAPI.?)
 * FIXME
 *     I haven't been able to find the ordinal for this function,
 *     This means it can't be called from outside the DLL.
 */
BOOL WINAPI QueryWorkingSet(HANDLE hProcess, LPVOID pv, DWORD cb)
{
#ifdef DEBUG
  dprintf(("PSAPI: QueryWorkingSet not implemented\n"));
#endif
  if(pv && cb)
    ((DWORD *) pv)[0] = 0; /* Empty WorkingSet */

  return TRUE;
}
