/* $Id: win32api.h,v 1.6 2000-03-22 18:47:01 sandervl Exp $ */

/*
 *
 * Win32 api include file for sources that include OS/2 headers
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __WIN32API_H__
#define __WIN32API_H__

#include <win32type.h>
#include <winconst.h>

void   WIN32API SetLastError(DWORD dwError);
int    WIN32API GetLastError(void);

LONG   WIN32API RegCloseKey(HKEY);
LONG   WIN32API RegFlushKey(HKEY);
LONG   WIN32API RegCreateKeyA(HKEY,LPCSTR,LPHKEY);
LONG   WIN32API RegCreateKeyW(HKEY,LPCWSTR,LPHKEY);
LONG   WIN32API RegCreateKeyExA(HKEY,LPCSTR,DWORD,LPSTR,DWORD,REGSAM,
                                LPSECURITY_ATTRIBUTES,LPHKEY,LPDWORD);
LONG   WIN32API RegCreateKeyExW(HKEY,LPCWSTR,DWORD,LPWSTR,DWORD,REGSAM,
                                LPSECURITY_ATTRIBUTES,LPHKEY,LPDWORD);
LONG   WIN32API RegDeleteKeyA(HKEY,LPCSTR);
LONG   WIN32API RegDeleteKeyW(HKEY,LPWSTR);
LONG   WIN32API RegDeleteValueA(HKEY,LPSTR);
LONG   WIN32API RegDeleteValueW(HKEY,LPWSTR);
LONG   WIN32API RegEnumKeyA(HKEY,DWORD,LPSTR,DWORD);
LONG   WIN32API RegEnumKeyW(HKEY,DWORD,LPWSTR,DWORD);
LONG   WIN32API RegEnumValueA(HKEY,DWORD,LPSTR,LPDWORD,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
LONG   WIN32API RegEnumValueW(HKEY,DWORD,LPWSTR,LPDWORD,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
LONG   WIN32API RegOpenKeyA(HKEY,LPCSTR,LPHKEY);
LONG   WIN32API RegOpenKeyW(HKEY,LPCWSTR,LPHKEY);
LONG   WIN32API RegOpenKeyExW(HKEY,LPCWSTR,DWORD,REGSAM,LPHKEY);
LONG   WIN32API RegOpenKeyExA(HKEY,LPCSTR,DWORD,REGSAM,LPHKEY);
LONG   WIN32API RegQueryValueA(HKEY,LPCSTR,LPSTR,LPLONG);
LONG   WIN32API RegQueryValueW(HKEY,LPCWSTR,LPWSTR,LPLONG);
LONG   WIN32API RegQueryValueExA(HKEY,LPSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
LONG   WIN32API RegQueryValueExW(HKEY,LPWSTR,LPDWORD,LPDWORD,LPBYTE,LPDWORD);
LONG   WIN32API RegSetValueA(HKEY,LPCSTR,DWORD,LPCSTR,DWORD);
LONG   WIN32API RegSetValueW(HKEY,LPCWSTR,DWORD,LPCWSTR,DWORD);
LONG   WIN32API RegSetValueExA(HKEY,LPSTR,DWORD,DWORD,LPBYTE,DWORD);
LONG   WIN32API RegSetValueExW(HKEY,LPWSTR,DWORD,DWORD,LPBYTE,DWORD);

DWORD  WIN32API GetModuleFileNameA(HMODULE,LPSTR,DWORD);

VOID   WIN32API ExitProcess(DWORD exitcode);

BOOL   WIN32API PostMessageA(HWND,UINT,WPARAM,LPARAM);
BOOL   WIN32API PostMessageW(HWND,UINT,WPARAM,LPARAM);

BOOL   WIN32API IsWindow(HWND);

DWORD  WIN32API GetTickCount(void);

HWND   WIN32API WindowFromDC(HDC hdc);
BOOL   WIN32API UnmapViewOfFile(LPVOID addr);
LPVOID WIN32API MapViewOfFile(HANDLE mapping, DWORD access, DWORD offset_high,
                              DWORD offset_low, DWORD count);

HANDLE WIN32API GetCurrentProcess(void);
DWORD  WIN32API GetCurrentProcessId(void);
HANDLE WIN32API GetCurrentThread(void);
DWORD  WIN32API GetCurrentThreadId(void);
HANDLE WIN32API GetProcessHeap(void);

LPVOID WIN32API HeapAlloc(HANDLE,DWORD,DWORD);
DWORD  WIN32API HeapCompact(HANDLE,DWORD);
HANDLE WIN32API HeapCreate(DWORD,DWORD,DWORD);
BOOL   WIN32API HeapDestroy(HANDLE);
BOOL   WIN32API HeapFree(HANDLE,DWORD,LPVOID);
BOOL   WIN32API HeapLock(HANDLE);
LPVOID WIN32API HeapReAlloc(HANDLE,DWORD,LPVOID,DWORD);
DWORD  WIN32API HeapSize(HANDLE,DWORD,LPVOID);
BOOL   WIN32API HeapUnlock(HANDLE);
BOOL   WIN32API HeapValidate(HANDLE,DWORD,LPCVOID);

#endif