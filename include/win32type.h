/* $Id: win32type.h,v 1.33 2000-02-29 19:09:25 sandervl Exp $ */

/*
 * Win32 type definitions for OS/2
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1998 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


#if !defined(__WIN32TYPE_H__) && !defined(_OS2WIN_H) && !defined(__INCLUDE_WINUSER_H) && !defined(__WINE_WINBASE_H)
#define __WIN32TYPE_H__

#include <odin.h>

#pragma pack(1)

// linkage macros
#define NEAR
#define FAR
#define UNALIGNED
#define CONST   const


/*****************************************************************************
 * Base types (already defined in OS2DEF.H)                                  *
 *****************************************************************************/

#ifndef OS2DEF_INCLUDED
// void
#undef VOID
#undef PVOID
typedef void            VOID;
typedef void*           PVOID;

// 32 bit integers
#undef LONG
#undef PLONG
#undef PCLONG
typedef                long   LONG;
typedef         signed long*  PLONG;
typedef const   signed long*  PCLONG;

#undef ULONG
#undef PULONG
#undef PCULONG
typedef       unsigned long   ULONG;
typedef       unsigned long*  PULONG;
typedef const unsigned long*  PCULONG;

#undef INT32
#undef PINT32
#undef PCINT32
typedef       unsigned int    INT32;
typedef       unsigned int *  PINT32;
typedef const unsigned int *  PCINT32;

#undef UINT32
#undef PUINT32
#undef PCUINT32
typedef       unsigned int    UINT32;
typedef       unsigned int *  PUINT32;
typedef const unsigned int *  PCUINT32;

#undef UINT
#undef PUINT
#undef PCUINT
typedef       unsigned int    UINT;
typedef       unsigned int *  PUINT;
typedef const unsigned int *  PCUINT;


// 16 bit integers
#undef SHORT
#undef PSHORT
#undef PCSHORT
typedef                short  SHORT;
typedef         signed short* PSHORT;
typedef const   signed short* PCSHORT;

#undef USHORT
#undef PUSHORT
#undef PCUSHORT
typedef       unsigned short  USHORT;
typedef       unsigned short* PUSHORT;
typedef const unsigned short* PCUSHORT;

#undef UINT16
#undef PUINT16
#undef PCUINT16
typedef       unsigned short  UINT16;
typedef       unsigned short* PUINT16;
typedef const unsigned short* PCUINT16;


// 8 bit integers
#undef UCHAR
#undef PUCHAR
#undef PCUCHAR
typedef       unsigned char  UCHAR;
typedef       unsigned char* PUCHAR;
typedef const unsigned char* PCUCHAR;

#undef UINT8
#undef PUINT8
#undef PCUINT8
typedef       unsigned char   UINT8;
typedef       unsigned char*  PUINT8;
typedef const unsigned char*  PCUINT8;

#undef INT8
#undef PINT8
#undef PCINT8
typedef         signed char   INT8;
typedef         signed char*  PINT8;
typedef const   signed char*  PCINT8;

#undef BYTE
#undef PBYTE
#undef PCBYTE
typedef       unsigned char  BYTE;
typedef       unsigned char* PBYTE;
typedef const unsigned char* PCBYTE;

// floats
#undef FLOAT
#undef PFLOAT
#undef LPFLOAT
typedef float FLOAT, *PFLOAT, *LPFLOAT;

#endif


/*****************************************************************************
 * Common types                                                              *
 *****************************************************************************/

#if !(defined(__WINE_WINDEF_H) || defined(OS2DEF_INCLUDED))

#undef INT
#undef PINT
#undef PCINT
typedef       unsigned int    INT;
typedef       unsigned int *  PINT;
typedef const unsigned int *  PCINT;

#undef CHAR
#undef PCHAR
#undef PCCHAR
typedef                char   CHAR;
typedef         signed char*  PCHAR;
typedef const   signed char*  PCCHAR;


#endif

/*****************************************************************************
 * Win32 types                                                               *
 *****************************************************************************/

#ifndef __WINE_WINDEF_H
// void
#undef LPVOID
#undef LPCVOID
typedef void*           LPVOID;
typedef const void*     LPCVOID;

// 32 bit integers
#undef LPLONG
#undef LPCLONG
typedef         signed long*  LPLONG;
typedef const   signed long*  LPCLONG;

#undef LPULONG
#undef LPCULONG
typedef       unsigned long*  LPULONG;
typedef const unsigned long*  LPCULONG;

#undef LPINT32
#undef LPCINT32
typedef       unsigned int *  LPINT32;
typedef const unsigned int *  LPCINT32;

#undef LPINT
#undef LPCINT
typedef       unsigned int *  LPINT;
typedef const unsigned int *  LPCINT;

#undef LPUINT32
#undef LPCUINT32
typedef       unsigned int *  LPUINT32;
typedef const unsigned int *  LPCUINT32;

#undef LPUINT
#undef LPCUINT
typedef       unsigned int *  LPUINT;
typedef const unsigned int *  LPCUINT;


// 16 bit integers
#undef INT16
#undef PINT16
#undef PCINT16
typedef       unsigned short  INT16;
typedef       unsigned short* PINT16;
typedef const unsigned short* PCINT16;

#undef LPUINT16
#undef LPCUINT16
typedef       unsigned short* LPUINT16;
typedef const unsigned short* LPCUINT16;

#undef LPSHORT
#undef LPCSHORT
typedef         signed short* LPSHORT;
typedef const   signed short* LPCSHORT;

#undef LPUSHORT
#undef LPCUSHORT
typedef       unsigned short* LPUSHORT;
typedef const unsigned short* LPCUSHORT;

#undef LPINT16
#undef LPCINT16
typedef       unsigned short* LPINT16;
typedef const unsigned short* LPCINT16;

// 8 bit integers
#undef LPCHAR
#undef LPCCHAR
typedef         signed char*  LPCHAR;
typedef const   signed char*  LPCCHAR;

#undef LPUCHAR
#undef LPCUCHAR
typedef       unsigned char* LPUCHAR;
typedef const unsigned char* LPCUCHAR;

#undef LPUINT8
#undef LPCUINT8
typedef       unsigned char*  LPUINT8;
typedef const unsigned char*  LPCUINT8;

#undef LPINT8
#undef LPCINT8
typedef         signed char*  LPINT8;
typedef const   signed char*  LPCINT8;

#undef LPBYTE
#undef LPCBYTE
typedef       unsigned char* LPBYTE;
typedef const unsigned char* LPCBYTE;

// floats
#undef FLOAT
#undef PFLOAT
#undef LPFLOAT
typedef float FLOAT, *PFLOAT, *LPFLOAT;

// doubles
typedef double          DOUBLE;
typedef double          LONGLONG_W;
typedef double          ULONGULONG_W;
typedef double          DWORDLONG_W;
typedef ULONGULONG_W    *PULONGULONG_W;
typedef DWORDLONG_W     *PDWORDLONG_W;

// boolean
typedef unsigned long   BOOL32;
typedef unsigned short  BOOL16;
typedef unsigned long   BOOL;


// calling conventions / linkage
#ifndef WINAPI
#  error Please include ODIN.H
#endif

#ifndef WIN32API
#  error Please include ODIN.H
#endif


// 32 bit integer
#undef DWORD
#undef PDWORD
#undef LPDWORD
#undef PCDWORD
#undef LPCDWORD
typedef       unsigned long   DWORD;
typedef       unsigned long*  PDWORD;
typedef       unsigned long*  LPDWORD;
typedef const unsigned long*  PCDWORD;
typedef const unsigned long*  LPCDWORD;

typedef DWORD LRESULT;


// 16 bit integer
#undef WORD
#undef PWORD
#undef LPWORD
#undef PCWORD
#undef LPCWORD
typedef       unsigned short  WORD;
typedef       unsigned short* PWORD;
typedef       unsigned short* LPWORD;
typedef const unsigned short* PCWORD;
typedef const unsigned short* LPCWORD;



// strings
/* Some systems might have wchar_t, but we really need 16 bit characters */
typedef unsigned short  WCHAR;
#define LPTSTR  char *
#define LPSTR   char *
#define LPCSTR  const char *
#define LPCTSTR const char *
#define LPWSTR  WCHAR *
#define LPCWSTR const WCHAR *

// handles
#define HANDLE  ULONG
#define PHANDLE HANDLE *
#define HINSTANCE ULONG
#define HGLOBAL DWORD
#define ATOM    DWORD
#define HRSRC   DWORD
#define HICON   DWORD
#define HCURSOR DWORD
#define HBRUSH  DWORD
#define HMENU   DWORD
#define HFONT   DWORD
#define WNDPROC DWORD
#define DLGPROC DWORD
#define WNDPROC_O32 DWORD
#define HRESULT DWORD
#define HFILE   DWORD
#define HQUEUE  HANDLE
typedef HANDLE  HHOOK;

//Wine types
#define HMODULE32 HMODULE
#define HWND16  WORD
#define HWND32  DWORD
#define LPUINT16 WORD *
#define HTASK16  WORD
#define HMMIO16  WORD
#define HTASK32  DWORD
#define HMMIO32  DWORD
#define SEGPTR   DWORD
#define RECT16   RECT
#define RECT32   RECT
#define HDC16    WORD
#define HDC32    DWORD
#define HANDLE16 WORD
#define HDRV16   WORD
#define HINSTANCE16 WORD
#define HQUEUE16 WORD

// other
#ifndef HWND
#  define HWND HWND32
#endif

typedef LONG    LPARAM;
typedef UINT    WPARAM;

typedef double          DATE;
typedef long            LONG_PTR;
typedef unsigned long   ULONG_PTR;


#define DECLSPEC_IMPORT


typedef struct tagSIZE
{
    INT  cx;
    INT  cy;
} SIZE, *PSIZE, *LPSIZE;

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

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard
} GET_FILEEX_INFO_LEVELS;

typedef struct _WIN32_FILE_ATTRIBUTES_DATA {
    DWORD    dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD    nFileSizeHigh;
    DWORD    nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;


#define CALLBACK WIN32API

typedef LRESULT (* CALLBACK FARPROC)();
typedef LRESULT (* CALLBACK FARPROC16)();

typedef LRESULT (* CALLBACK HOOKPROC)(INT,WPARAM,LPARAM);

typedef union _LARGE_INTEGER {
    struct {
        ULONG LowPart;
        LONG HighPart;
    } u;
    //@@@PH this can't really work ...
    ULONGULONG_W QuadPart;
} LARGE_INTEGER;


typedef struct
{
    UINT      cbSize;
    UINT      style;
    WNDPROC   lpfnWndProc;
    INT       cbClsExtra;
    INT       cbWndExtra;
    HINSTANCE hInstance;
    HICON     hIcon;
    HCURSOR   hCursor;
    HBRUSH    hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
    HICON     hIconSm;
} WNDCLASSEXA, *LPWNDCLASSEXA;

typedef struct
{
    UINT      cbSize;
    UINT      style;
    WNDPROC   lpfnWndProc;
    INT       cbClsExtra;
    INT       cbWndExtra;
    HINSTANCE hInstance;
    HICON     hIcon;
    HCURSOR   hCursor;
    HBRUSH    hbrBackground;
    LPCWSTR     lpszMenuName;
    LPCWSTR     lpszClassName;
    HICON     hIconSm;
} WNDCLASSEXW, *LPWNDCLASSEXW;


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


#ifndef RECT
//conflict with mcios2.h

typedef struct tagRECT
{
    INT  left;
    INT  top;
    INT  right;
    INT  bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT *LPCRECT;
#endif


/* WM_WINDOWPOSCHANGING/CHANGED struct */
typedef struct tagWINDOWPOS
{
    HWND  hwnd;
    HWND  hwndInsertAfter;
    INT   x;
    INT   y;
    INT   cx;
    INT   cy;
    UINT  flags;
} WINDOWPOS, *PWINDOWPOS, *LPWINDOWPOS;


typedef struct tagCREATESTRUCTA
{
    LPVOID      lpCreateParams;
    HINSTANCE hInstance;
    HMENU     hMenu;
    HWND      hwndParent;
    INT       cy;
    INT       cx;
    INT       y;
    INT       x;
    LONG        style;
    LPCSTR      lpszName;
    LPCSTR      lpszClass;
    DWORD       dwExStyle;
} CREATESTRUCTA, *LPCREATESTRUCTA;

typedef struct
{
    LPVOID      lpCreateParams;
    HINSTANCE hInstance;
    HMENU     hMenu;
    HWND      hwndParent;
    INT       cy;
    INT       cx;
    INT       y;
    INT       x;
    LONG        style;
    LPCWSTR     lpszName;
    LPCWSTR     lpszClass;
    DWORD       dwExStyle;
} CREATESTRUCTW, *LPCREATESTRUCTW;
#endif



/*****************************************************************************
 * Additional types                                                          *
 *****************************************************************************/

//PH: Warning -- OS/2  has BOOL == unsigned long,
//               Win32 has BOOL == int.
typedef int             BOOL32_W;
typedef int             BOOL_W;







//@@@PH -----------------------------------------------------------------------
#if 0


#define DECLARE_HANDLE(x) typedef DWORD x

DECLARE_HANDLE(HTASK);

#ifndef WIN32API
#define WIN32API __stdcall
#endif


typedef struct _MEMORY_BASIC_INFORMATION
{
    LPVOID   BaseAddress;
    LPVOID   AllocationBase;
    DWORD    AllocationProtect;
    DWORD    RegionSize;
    DWORD    State;
    DWORD    Protect;
    DWORD    Type;
} MEMORY_BASIC_INFORMATION,*PMEMORY_BASIC_INFORMATION,*LPMEMORY_BASIC_INFORMATION;

#endif

/* Could this type be considered opaque? */
#ifndef __WINE_WINBASE_H

typedef struct {
	LPVOID	DebugInfo;
	LONG LockCount;
	LONG RecursionCount;
	HANDLE OwningThread;
	HANDLE LockSemaphore;
	DWORD Reserved;
}CRITICAL_SECTION;

/* The 'overlapped' data structure used by async I/O functions.
 */
typedef struct {
        DWORD Internal;
        DWORD InternalHigh;
        DWORD Offset;
        DWORD OffsetHigh;
        HANDLE hEvent;
} OVERLAPPED, *LPOVERLAPPED;

#endif

/* Scrollbar info */
#ifndef __INCLUDE_WINUSER_H
typedef struct
{
    UINT    cbSize;
    UINT    fMask;
    INT     nMin;
    INT     nMax;
    UINT    nPage;
    INT     nPos;
    INT     nTrackPos;
} SCROLLINFO, *LPSCROLLINFO;
typedef SCROLLINFO CONST *LPCSCROLLINFO;
#endif

#ifndef __INCLUDE_WINUSER_H
/* WM_GETMINMAXINFO struct */
typedef struct
{
    POINT   ptReserved;
    POINT   ptMaxSize;
    POINT   ptMaxPosition;
    POINT   ptMinTrackSize;
    POINT   ptMaxTrackSize;
} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;
#endif


#ifndef __INCLUDE_WINUSER_H
typedef struct
{
    UINT   length;
    UINT   flags;
    UINT   showCmd;
    POINT  ptMinPosition;
    POINT  ptMaxPosition;
    RECT   rcNormalPosition;
} WINDOWPLACEMENT, *LPWINDOWPLACEMENT;

typedef BOOL    (* CALLBACK WNDENUMPROC)(HWND,LPARAM);

#endif

typedef DWORD (* CALLBACK LPTHREAD_START_ROUTINE)(LPVOID);
/* typedef struct _EXCEPTION_RECORD {
    DWORD   ExceptionCode;
    DWORD   ExceptionFlags;
    struct  _EXCEPTION_RECORD *ExceptionRecord;
    LPVOID  ExceptionAddress;
    DWORD   NumberParameters;
    DWORD   ExceptionInformation[15];
} EXCEPTION_RECORD; */

typedef struct _EXCEPTION_DEBUG_INFO {
/*    EXCEPTION_RECORD ExceptionRecord; */
    DWORD dwFirstChange;
} EXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID   lpBaseOfDll;
    DWORD    dwDebugInfoFileOffset;
    DWORD    nDebugInfoSize;
    LPVOID   lpImageName;
    WORD     fUnicode;
} LOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD  fUnicode;
    WORD  nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO;

typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO      Exception;
        CREATE_THREAD_DEBUG_INFO  CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO    ExitThread;
        EXIT_PROCESS_DEBUG_INFO   ExitProcess;
        LOAD_DLL_DEBUG_INFO       LoadDll;
        UNLOAD_DLL_DEBUG_INFO     UnloadDll;
        OUTPUT_DEBUG_STRING_INFO  DebugString;
        RIP_INFO                  RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;

#ifndef __WINE_WINNT_H

#pragma pack(1)
#define ANYSIZE_ARRAY 1

#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED
#define SID_IDENTIFIER_AUTHORITY_DEFINED
typedef struct {
    BYTE Value[6];
} SID_IDENTIFIER_AUTHORITY,*PSID_IDENTIFIER_AUTHORITY,*LPSID_IDENTIFIER_AUTHORITY;
#endif /* !defined(SID_IDENTIFIER_AUTHORITY_DEFINED) */

#ifndef SID_DEFINED
#define SID_DEFINED
typedef struct _SID {
    BYTE Revision;
    BYTE SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    DWORD SubAuthority[1];
} SID,*PSID;
#endif /* !defined(SID_DEFINED) */

/* 
 * ACL 
 */

typedef struct _ACL {
    BYTE AclRevision;
    BYTE Sbz1;
    WORD AclSize;
    WORD AceCount;
    WORD Sbz2;
} ACL, *PACL;

/* 
 * SID_AND_ATTRIBUTES
 */

typedef struct _SID_AND_ATTRIBUTES {
  PSID  Sid; 
  DWORD Attributes; 
} SID_AND_ATTRIBUTES ; 
 
/*
 * TOKEN_USER
 */

typedef struct _TOKEN_USER {
  SID_AND_ATTRIBUTES User; 
} TOKEN_USER; 

/*
 * TOKEN_GROUPS
 */

typedef struct _TOKEN_GROUPS  {
  DWORD GroupCount; 
  SID_AND_ATTRIBUTES Groups[ANYSIZE_ARRAY]; 
} TOKEN_GROUPS; 

typedef LARGE_INTEGER LUID,*PLUID;

typedef struct _LUID_AND_ATTRIBUTES {
  LUID   Luid; 
  DWORD  Attributes; 
} LUID_AND_ATTRIBUTES; 

/*
 * PRIVILEGE_SET
 */

typedef struct _PRIVILEGE_SET {
    DWORD PrivilegeCount;
    DWORD Control;
    LUID_AND_ATTRIBUTES Privilege[ANYSIZE_ARRAY];
} PRIVILEGE_SET, *PPRIVILEGE_SET;

/*
 * TOKEN_PRIVILEGES
 */

typedef struct _TOKEN_PRIVILEGES {
  DWORD PrivilegeCount; 
  LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY]; 
} TOKEN_PRIVILEGES, *PTOKEN_PRIVILEGES; 

/*
 * TOKEN_OWNER
 */

typedef struct _TOKEN_OWNER {
  PSID Owner; 
} TOKEN_OWNER; 

/*
 * TOKEN_PRIMARY_GROUP
 */

typedef struct _TOKEN_PRIMARY_GROUP {
  PSID PrimaryGroup; 
} TOKEN_PRIMARY_GROUP; 


/*
 * TOKEN_DEFAULT_DACL
 */

typedef struct _TOKEN_DEFAULT_DACL { 
  PACL DefaultDacl; 
} TOKEN_DEFAULT_DACL; 

/*
 * TOKEN_SOURCEL
 */

typedef struct _TOKEN_SOURCE {
  char Sourcename[8]; 
  LUID SourceIdentifier; 
} TOKEN_SOURCE; 

/*
 * TOKEN_TYPE
 */

typedef enum tagTOKEN_TYPE {
  TokenPrimary = 1, 
  TokenImpersonation 
} TOKEN_TYPE; 

/*
 * SECURITY_IMPERSONATION_LEVEL
 */

typedef enum _SECURITY_IMPERSONATION_LEVEL {
  SecurityAnonymous, 
  SecurityIdentification, 
  SecurityImpersonation, 
  SecurityDelegation 
} SECURITY_IMPERSONATION_LEVEL, *PSECURITY_IMPERSONATION_LEVEL; 

#pragma pack()

#endif //__WINE_WINNT_H

#pragma pack()

#endif
