/* $Id: win32type.h,v 1.9 1999-07-17 11:50:26 sandervl Exp $ */

#ifndef __WIN32TYPE_H__
#define __WIN32TYPE_H__

#include <odin.h>

#define NEAR
#define FAR
#define UNALIGNED


#ifndef VOID
#  define VOID void
#endif

#ifndef LONG
#  define LONG long
#endif

#ifndef ULONG
#  define ULONG unsigned long
#endif

#ifndef SHORT
#  define SHORT short
#endif

#ifndef USHORT
#  define USHORT unsigned short
#endif

#ifndef CHAR
#  define CHAR char
#endif

#ifndef UCHAR
#  define UCHAR unsigned char
#endif



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
#define WNDPROC_O32 DWORD
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
typedef double    ULONGULONG;
typedef double     DWORDLONG;
typedef ULONGULONG  *PULONGULONG;
typedef DWORDLONG *PDWORDLONG;
#define BOOL	  int

#define DECLSPEC_IMPORT

#define DECLARE_HANDLE(x) typedef DWORD x

typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        LONG HighPart;
    } u;
    ULONGULONG QuadPart;
} LARGE_INTEGER;

/* The security attributes structure */
typedef struct
{
    DWORD   nLength;
    LPVOID  lpSecurityDescriptor;
    BOOL  bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#define OFS_MAXPATHNAME 128
typedef struct
{
    BYTE cBytes;
    BYTE fFixedDisk;
    WORD nErrCode;
    BYTE reserved[4];
    BYTE szPathName[OFS_MAXPATHNAME];
} OFSTRUCT, *LPOFSTRUCT;

#ifndef _FILETIME_
#define _FILETIME_
/* 64 bit number of 100 nanoseconds intervals since January 1, 1601 */
typedef struct
{
  DWORD  dwLowDateTime;
  DWORD  dwHighDateTime;
} FILETIME, *LPFILETIME;
#endif /* _FILETIME_ */

typedef struct
{
  int dwFileAttributes;
  FILETIME ftCreationTime;
  FILETIME ftLastAccessTime;
  FILETIME ftLastWriteTime;
  int dwVolumeSerialNumber;
  int nFileSizeHigh;
  int nFileSizeLow;
  int nNumberOfLinks;
  int nFileIndexHigh;
  int nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION ;

DECLARE_HANDLE(HTASK);

#ifndef WIN32API
#define WIN32API __stdcall
#endif

#define CALLBACK WIN32API

typedef LRESULT (* CALLBACK FARPROC)();
typedef LRESULT (* CALLBACK FARPROC16)();

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, *LPPOINT;

typedef struct
{
    HWND    hwnd;
    UINT    message;
    WPARAM  wParam;
    LPARAM  lParam;
    DWORD   time;
    POINT   pt;
} MSG, *LPMSG;

#endif
