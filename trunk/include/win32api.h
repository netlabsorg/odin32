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
DWORD  WIN32API GetLastError(void);

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
LRESULT WIN32API SendMessageA(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WIN32API SendMessageW(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

BOOL   WIN32API DosDateTimeToFileTime(WORD,WORD,LPFILETIME);


HWND   WIN32API GetDesktopWindow(void);
INT    WIN32API GetUpdateRgn(HWND, HRGN, BOOL);
BOOL   WIN32API HideCaret(HWND hwnd);
BOOL   WIN32API ShowCaret(HWND hwnd);
HDC    WIN32API GetDCEx(HWND hwnd, HRGN hrgn, ULONG flags);
int    WIN32API ReleaseDC(HWND hwnd, HDC hdc);
int    WIN32API ExtSelectClipRgn(HDC hdc, HRGN hrgn, int fnMode);
int    WIN32API SelectClipRgn( HDC hdc, HRGN hRgn);
BOOL   WIN32API GetWindowRect( HWND hwnd, RECT *pRect);
BOOL   WIN32API GetClientRect( HWND hwnd, RECT *pRect);
int    WIN32API GetStretchBltMode( HDC hdc);

HRSRC   WIN32API FindResourceA(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType);
HRSRC   WIN32API FindResourceW(HINSTANCE hModule, LPCSTR lpszName, LPCSTR lpszType);
HGLOBAL WIN32API LoadResource(HINSTANCE hModule, HRSRC hRes);
DWORD   WIN32API SizeofResource(HINSTANCE hModule, HRSRC hRes);
PVOID   WIN32API LockResource(HGLOBAL hRes);

BOOL    WIN32API SetWindowOrgEx(HDC hdc, int X, int Y, LPPOINT lpPoint);
BOOL    WIN32API SetWindowExtEx(HDC hdc, int nXExtent, int nYExtent, LPSIZE lpSize);
int     WIN32API GetDeviceCaps(HDC hdc, int nIndex);
int     WIN32API GetMapMode(HDC hdc);
BOOL    WIN32API DPtoLP(HDC,LPPOINT,INT);
BOOL    WIN32API LPtoDP(HDC,LPPOINT,INT);
BOOL    WIN32API RedrawWindow(HWND,const RECT*,HRGN, DWORD);
HGDIOBJ WIN32API SelectObject(HDC,HGDIOBJ);
BOOL    WIN32API DeleteObject(HANDLE hObj);
DWORD   WIN32API GetRegionData(HRGN,DWORD,LPRGNDATA);
BOOL    WIN32API SetRectRgn(HRGN,INT,INT,INT,INT);
HRGN    WIN32API CreateRectRgn(INT,INT,INT,INT);
HRGN    WIN32API CreateRectRgnIndirect(const RECT*);
HRGN    WIN32API CreateRoundRectRgn(INT,INT,INT,INT,INT,INT);
INT     WIN32API CombineRgn(HRGN,HRGN,HRGN,INT);
BOOL    WIN32API SetRectRgn(HRGN,INT,INT,INT,INT);
BOOL    WIN32API EnumChildWindows(HWND hwnd, WNDENUMPROC lpfn, LPARAM lParam);

LPCSTR  WIN32API GetCommandLineA(VOID);
LPCWSTR WIN32API GetCommandLineW(VOID);

BOOL    WIN32API SetCurrentDirectoryA(LPCSTR lpstrDirectory);
UINT    WIN32API GetCurrentDirectoryA(UINT nBufferLength, LPSTR lpBuffer);

HANDLE  WIN32API GetStdHandle(DWORD a);
DWORD   WIN32API GetFileType(HANDLE hFile);

BOOL    WIN32API SetWindowPos(HWND,HWND,INT,INT,INT,INT,UINT);
HWND    WIN32API GetWindow(HWND hwnd, UINT uCmd);
BOOL    WIN32API IntersectRect( PRECT lprcDst, const RECT * lprcSrc1, const RECT * lprcSrc2);
BOOL    WIN32API OffsetRect( PRECT lprc, int x, int  y);

DWORD   WIN32API GetTickCount(void);
DWORD   WIN32API GetCurrentThreadId();

INT     WIN32API ShowCursor(BOOL);

BOOL    WIN32API PostThreadMessageA(DWORD, UINT, WPARAM, LPARAM);
BOOL    WIN32API PostThreadMessageW(DWORD, UINT, WPARAM, LPARAM);

BOOL    WIN32API ResetEvent(HANDLE);
BOOL    WIN32API SetEvent(HANDLE);
BOOL    WIN32API PulseEvent (HANDLE hEvent);

HANDLE  WIN32API CreateThread(LPSECURITY_ATTRIBUTES,DWORD,LPTHREAD_START_ROUTINE,LPVOID,DWORD,LPDWORD);
VOID    WIN32API ExitThread(DWORD dwExitCode);
BOOL    WIN32API TerminateThread(HANDLE hThread, DWORD dwExitCode);

HWND    WIN32API WindowFromPoint(POINT);
INT     WIN32API MapWindowPoints(HWND,HWND,LPPOINT,UINT);
HWND    WIN32API GetCapture(void);
HWND    WIN32API SetCapture(HWND);
BOOL    WIN32API IsWindowEnabled(HWND);
HWND    WIN32API SetActiveWindow( HWND hwnd);

HMODULE WIN32API LoadLibraryA(LPCSTR);
BOOL    WIN32API FreeLibrary(HMODULE);
FARPROC WIN32API GetProcAddress(HMODULE,LPCSTR);
VOID    WIN32API DeleteCriticalSection(CRITICAL_SECTION *);
VOID    WIN32API EnterCriticalSection(CRITICAL_SECTION *);
VOID    WIN32API InitializeCriticalSection(CRITICAL_SECTION *);
VOID    WIN32API LeaveCriticalSection(CRITICAL_SECTION *);
VOID    WIN32API MakeCriticalSectionGlobal(CRITICAL_SECTION *);
LONG    WIN32API InterlockedDecrement(LPLONG);
LONG    WIN32API InterlockedExchange(LPLONG,LONG);
LONG    WIN32API InterlockedIncrement(LPLONG);

UINT    WIN32API GetSystemDirectoryA(LPSTR,UINT);
UINT    WIN32API GetWindowsDirectoryA(LPSTR,UINT);

BOOL    WIN32API IsIconic( HWND hwnd);
INT     WIN32API GetSystemMetrics(INT);
UINT    WIN32API GetDoubleClickTime(void);

UINT    WIN32API SetErrorMode(UINT);

UINT    WIN32API GetDriveTypeA(LPCSTR);

BOOL    WIN32API SetProcessAffinityMask( HANDLE hProcess, DWORD affmask );
BOOL    WIN32API GetProcessAffinityMask(HANDLE  hProcess,
                                        LPDWORD lpProcessAffinityMask,
                                        LPDWORD lpSystemAffinityMask);

#endif
