/* $Id: win32type.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

#ifndef __WIN32TYPE_H__
#define __WIN32TYPE_H__

#define NEAR
#define FAR
#define UNALIGNED

#define LPVOID  VOID *
#define LPCVOID const VOID *
#define DWORD   ULONG
#define LPDWORD DWORD *
#define LPLONG  LONG *
#define PDWORD  DWORD *
#define LRESULT DWORD
#define LPARAM  DWORD
#define WPARAM  DWORD
#define CONST   const
#define LPTSTR  char *
#define LPSTR   char *
#define LPCSTR  const char *
#define LPBYTE  unsigned char *
#define HANDLE  ULONG
#define WORD    USHORT
#define LPWORD  WORD *
#define PWORD   WORD *
#define LPCWORD const WORD *
#define WCHAR   USHORT
#define LPWSTR  WCHAR *
#define LPCWSTR const WCHAR *
#define HINSTANCE ULONG
#define HGLOBAL DWORD
#define HRSRC   DWORD
#define HICON   DWORD
#define HCURSOR DWORD
#define HBRUSH  DWORD
#define HMENU   DWORD
#define WNDPROC DWORD
#define HRESULT DWORD

//Wine types
#define UINT16  WORD
#define UINT32  DWORD
#define BOOL16  WORD
#define HMODULE32 HMODULE
#define HWND16  WORD
#define HWND32  DWORD
#define LPUINT16 WORD *
#define LPUINT32 DWORD *
#define HTASK16  WORD
#define HMMIO16  WORD
#define HTASK32  DWORD
#define HMMIO32  DWORD
#define INT16    short int
#define INT32    int
#define SEGPTR   DWORD
#define RECT16   RECT
#define RECT32   RECT
#define HDC16    WORD
#define HDC32    DWORD
#define HANDLE16 WORD
#define HDRV16   WORD
#define HINSTANCE16 WORD
typedef double     LONGLONG;
typedef double     DWORDLONG;
typedef LONGLONG  *PLONGLONG;
typedef DWORDLONG *PDWORDLONG;
#define BOOL	  int

#define DECLSPEC_IMPORT

#define DECLARE_HANDLE(x) typedef DWORD x

typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;

DECLARE_HANDLE(HTASK);

#ifndef WIN32API
#define WIN32API __stdcall
#endif

#define CALLBACK WIN32API

typedef LRESULT (* CALLBACK FARPROC)();
typedef LRESULT (* CALLBACK FARPROC16)();

#endif
