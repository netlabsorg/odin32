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
#define PWSTR   WCHAR *
#define LPCWSTR const WCHAR *

// handles
#define HANDLE  ULONG
#define PHANDLE HANDLE *
#define HINSTANCE ULONG
#define HGLOBAL DWORD
#define HGDIOBJ DWORD
#define ATOM    DWORD
#define HRSRC   DWORD
#define HICON   DWORD
#define HCURSOR DWORD
#define HBRUSH  DWORD
#define HPEN    DWORD
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

typedef unsigned long HKEY;
typedef HKEY* LPHKEY;
typedef DWORD REGSAM;

// other
#ifndef HWND
#  define HWND HWND32
#endif

typedef LONG    LPARAM;
typedef UINT    WPARAM;

typedef double          DATE;
typedef long            LONG_PTR;
typedef unsigned long   ULONG_PTR;

typedef BOOL    (* WIN32API PROPENUMPROCEXA)(HWND,LPCSTR,HANDLE,LPARAM);
typedef BOOL    (* WIN32API PROPENUMPROCEXW)(HWND,LPCWSTR,HANDLE,LPARAM);


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

typedef struct {
        WORD wYear;
        WORD wMonth;
        WORD wDayOfWeek;
        WORD wDay;
        WORD wHour;
        WORD wMinute;
        WORD wSecond;
        WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;

typedef struct
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    CHAR      cFileName[260];
    CHAR      cAlternateFileName[14];
} WIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct
{
    DWORD     dwFileAttributes;
    FILETIME  ftCreationTime;
    FILETIME  ftLastAccessTime;
    FILETIME  ftLastWriteTime;
    DWORD     nFileSizeHigh;
    DWORD     nFileSizeLow;
    DWORD     dwReserved0;
    DWORD     dwReserved1;
    WCHAR     cFileName[260];
    WCHAR     cAlternateFileName[14];
} WIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;

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
typedef VOID    (* CALLBACK LPMOUSE_EVENT_PROC)(DWORD,DWORD,DWORD,DWORD,DWORD);

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

typedef struct tagPOINT16
{
    SHORT  x;
    SHORT  y;
} POINT16, *PPOINT16, *LPPOINT16;

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
#else
typedef RECT *PRECT;
typedef const RECT *LPCRECT;
#endif


typedef struct _RGNDATAHEADER {
    DWORD dwSize;
    DWORD iType;
    DWORD nCount;
    DWORD nRgnSize;
    RECT  rcBound;
} RGNDATAHEADER,*LPRGNDATAHEADER;

typedef struct _RGNDATA {
    RGNDATAHEADER rdh;
    char    Buffer[1];
} RGNDATA,*PRGNDATA,*LPRGNDATA;

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
        LPVOID  DebugInfo;
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

typedef VOID (WIN32API *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD dwErrorCode,
                                                         DWORD dwNumberOfBytesTransfered,
                                                         LPOVERLAPPED lpOverlapped);

#endif

// COMport defines

#ifndef __WINE_WINBASE_H
/*
 Serial provider type
 */
#define SP_SERIALCOMM ((DWORD)0x00000001)
/*
 Provider SubTypes
 */
#define PST_UNSPECIFIED     ((DWORD)0x00000000)
#define PST_RS232           ((DWORD)0x00000001)
#define PST_PARALLELPORT    ((DWORD)0x00000002)
#define PST_RS422           ((DWORD)0x00000003)
#define PST_RS423           ((DWORD)0x00000004)
#define PST_RS449           ((DWORD)0x00000005)
#define PST_MODEM           ((DWORD)0x00000006)
#define PST_FAX             ((DWORD)0x00000021)
#define PST_SCANNER         ((DWORD)0x00000022)
#define PST_NETWORK_BRIDGE  ((DWORD)0x00000100)
#define PST_LAT             ((DWORD)0x00000101)
#define PST_TCPIP_TELNET    ((DWORD)0x00000102)
#define PST_X25             ((DWORD)0x00000103)
/*
 Provider Caps
 */
#define PCF_DTRDSR          ((DWORD)0x00000001)
#define PCF_RTSCTS          ((DWORD)0x00000002)
#define PCF_RLSD            ((DWORD)0x00000004)
#define PCF_PARITY_CHECK    ((DWORD)0x00000008)
#define PCF_XONXOFF         ((DWORD)0x00000010)
#define PCF_SETXCHAR        ((DWORD)0x00000020)
#define PCF_TOTALTIMEOUTS   ((DWORD)0x00000040)
#define PCF_INITTIMEOUTS    ((DWORD)0x00000080)
#define PCF_SPECIALCHARS    ((DWORD)0x00000100)
#define PCF_16BITMODE       ((DWORD)0x00000200)

/*
 Provider setable parameters
 */
#define SP_PARITY          ((DWORD)0x00000001)
#define SP_BAUD            ((DWORD)0x00000002)
#define SP_DATABITS        ((DWORD)0x00000004)
#define SP_STOPBIT         ((DWORD)0x00000008)
#define SP_HANDSHAKEING    ((DWORD)0x00000010)
#define SP_PARITY_CHECK    ((DWORD)0x00000020)
#define SP_RLSD            ((DWORD)0x00000040)
/*
 Settable baudrates in the provider
*/
#define BAUD_075           ((DWORD)0x00000001)
#define BAUD_110           ((DWORD)0x00000002)
#define BAUD_134_5         ((DWORD)0x00000004)
#define BAUD_150           ((DWORD)0x00000008)
#define BAUD_300           ((DWORD)0x00000010)
#define BAUD_600           ((DWORD)0x00000020)
#define BAUD_1200          ((DWORD)0x00000040)
#define BAUD_1800          ((DWORD)0x00000080)
#define BAUD_2400          ((DWORD)0x00000100)
#define BAUD_4800          ((DWORD)0x00000200)
#define BAUD_7200          ((DWORD)0x00000400)
#define BAUD_9600          ((DWORD)0x00000800)
#define BAUD_14400         ((DWORD)0x00001000)
#define BAUD_19200         ((DWORD)0x00002000)
#define BAUD_38400         ((DWORD)0x00004000)
#define BAUD_56K           ((DWORD)0x00008000)
#define BAUD_128K          ((DWORD)0x00010000)
#define BAUD_115200        ((DWORD)0x00020000)
#define BAUD_57600         ((DWORD)0x00040000)
#define BAUD_USER          ((DWORD)0x10000000)

#define CBR_110 0xFF10
#define CBR_300 0xFF11
#define CBR_600 0xFF12
#define CBR_1200        0xFF13
#define CBR_2400        0xFF14
#define CBR_4800        0xFF15
#define CBR_9600        0xFF16
#define CBR_14400       0xFF17
#define CBR_19200       0xFF18
#define CBR_38400       0xFF1B
#define CBR_56000       0xFF1F
#define CBR_128000      0xFF23
#define CBR_256000      0xFF27

/*
 Setable Databits
 */
#define DATABITS_5         ((DWORD)0x00000001)
#define DATABITS_6         ((DWORD)0x00000002)
#define DATABITS_7         ((DWORD)0x00000004)
#define DATABITS_8         ((DWORD)0x00000008)
#define DATABITS_16        ((DWORD)0x00000010)
#define DATABITS_16X       ((DWORD)0x00000020)

/*
 Setable Stop and Parity Bits
 */

#define STOPBITS_10       ((DWORD)0x00000001)
#define STOPBITS_15       ((DWORD)0x00000002)
#define STOPBITS_20       ((DWORD)0x00000004)
#define PARITY_NONE       ((DWORD)0x00000100)
#define PARITY_ODD        ((DWORD)0x00000200)
#define PARITY_EVEN       ((DWORD)0x00000400)
#define PARITY_MARK       ((DWORD)0x00000800)
#define PARITY_SPACE      ((DWORD)0x00001000)

#define STOPBITS_10       ((DWORD)0x00000001)
#define STOPBITS_15       ((DWORD)0x00000002)
#define STOPBITS_20       ((DWORD)0x00000004)
#define PARITY_NONE       ((DWORD)0x00000100)
#define PARITY_ODD        ((DWORD)0x00000200)
#define PARITY_EVEN       ((DWORD)0x00000400)
#define PARITY_MARK       ((DWORD)0x00000800)
#define PARITY_SPACE      ((DWORD)0x00001000)

#define NOPARITY        0
#define ODDPARITY       1
#define EVENPARITY      2
#define MARKPARITY      3
#define SPACEPARITY     4
#define ONESTOPBIT      0
#define ONE5STOPBITS    1
#define TWOSTOPBITS     2

/* Modem Status Flags */
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)

#define RTS_CONTROL_DISABLE     0
#define RTS_CONTROL_ENABLE      1
#define RTS_CONTROL_HANDSHAKE   2
#define RTS_CONTROL_TOGGLE      3

#define DTR_CONTROL_DISABLE     0
#define DTR_CONTROL_ENABLE      1
#define DTR_CONTROL_HANDSHAKE   2

#define CSTF_CTSHOLD    0x01
#define CSTF_DSRHOLD    0x02
#define CSTF_RLSDHOLD   0x04
#define CSTF_XOFFHOLD   0x08
#define CSTF_XOFFSENT   0x10
#define CSTF_EOF        0x20
#define CSTF_TXIM       0x40


#define IGNORE          0
#define INFINITE16      0xFFFF
#define INFINITE      0xFFFFFFFF

#define CE_RXOVER       0x0001
#define CE_OVERRUN      0x0002
#define CE_RXPARITY     0x0004
#define CE_FRAME        0x0008
#define CE_BREAK        0x0010
#define CE_CTSTO        0x0020
#define CE_DSRTO        0x0040
#define CE_RLSDTO       0x0080
#define CE_TXFULL       0x0100
#define CE_PTO          0x0200
#define CE_IOE          0x0400
#define CE_DNS          0x0800
#define CE_OOP          0x1000
#define CE_MODE 0x8000

#define IE_BADID        -1
#define IE_OPEN -2
#define IE_NOPEN        -3
#define IE_MEMORY       -4
#define IE_DEFAULT      -5
#define IE_HARDWARE     -10
#define IE_BYTESIZE     -11
#define IE_BAUDRATE     -12

#define EV_RXCHAR       0x0001
#define EV_RXFLAG       0x0002
#define EV_TXEMPTY      0x0004
#define EV_CTS          0x0008
#define EV_DSR          0x0010
#define EV_RLSD 0x0020
#define EV_BREAK        0x0040
#define EV_ERR          0x0080
#define EV_RING 0x0100
#define EV_PERR 0x0200
#define EV_CTSS 0x0400
#define EV_DSRS 0x0800
#define EV_RLSDS        0x1000
#define EV_RINGTE       0x2000
#define EV_RingTe       EV_RINGTE

#define SETXOFF 1
#define SETXON          2
#define SETRTS          3
#define CLRRTS          4
#define SETDTR          5
#define CLRDTR          6
#define RESETDEV        7
#define SETBREAK        8
#define CLRBREAK        9

#define GETBASEIRQ      10

/* Purge functions for Comm Port */
#define PURGE_TXABORT       0x0001  /* Kill the pending/current writes to the
                                       comm port */
#define PURGE_RXABORT       0x0002  /*Kill the pending/current reads to
                                     the comm port */
#define PURGE_TXCLEAR       0x0004  /* Kill the transmit queue if there*/
#define PURGE_RXCLEAR       0x0008  /* Kill the typeahead buffer if there*/



typedef struct _COMMPROP {
    WORD wPacketLength;
    WORD wPacketVersion;
    DWORD dwServiceMask;
    DWORD dwReserved1;
    DWORD dwMaxTxQueue;
    DWORD dwMaxRxQueue;
    DWORD dwMaxBaud;
    DWORD dwProvSubType;
    DWORD dwProvCapabilities;
    DWORD dwSettableParams;
    DWORD dwSettableBaud;
    WORD wSettableData;
    WORD wSettableStopParity;
    DWORD dwCurrentTxQueue;
    DWORD dwCurrentRxQueue;
    DWORD dwProvSpec1;
    DWORD dwProvSpec2;
    WCHAR wcProvChar[1];
} COMMPROP,*LPCOMMPROP;

#ifndef INCL_OS2MM

typedef struct tagCOMSTAT
{
    // DWORD status;
    unsigned fCtsHold:1;
    unsigned fDsrHold:1;
    unsigned fRlsdHold:1;
    unsigned fXoffHold:1;
    unsigned fXoffSend:1;
    unsigned fEof:1;
    unsigned fTxim:1;
    unsigned fReserved:25;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT,*LPCOMSTAT;

typedef struct tagDCB
{
    DWORD DCBlength;
    DWORD BaudRate;
    unsigned fBinary               :1;
    unsigned fParity               :1;
    unsigned fOutxCtsFlow          :1;
    unsigned fOutxDsrFlow          :1;
    unsigned fDtrControl           :2;
    unsigned fDsrSensitivity       :1;
    unsigned fTXContinueOnXoff     :1;
    unsigned fOutX                 :1;
    unsigned fInX                  :1;
    unsigned fErrorChar            :1;
    unsigned fNull                 :1;
    unsigned fRtsControl           :2;
    unsigned fAbortOnError         :1;
    unsigned fDummy2               :17;
    WORD wReserved;
    WORD XonLim;
    WORD XoffLim;
    BYTE ByteSize;
    BYTE Parity;
    BYTE StopBits;
    char XonChar;
    char XoffChar;
    char ErrorChar;
    char EofChar;
    char EvtChar;
} DCB, *LPDCB;



typedef struct tagCOMMTIMEOUTS {
        DWORD   ReadIntervalTimeout;
        DWORD   ReadTotalTimeoutMultiplier;
        DWORD   ReadTotalTimeoutConstant;
        DWORD   WriteTotalTimeoutMultiplier;
        DWORD   WriteTotalTimeoutConstant;
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

typedef struct _COMMCONFIG {
    DWORD dwSize;
    WORD wVersion;
    WORD wReserved;
    DCB dcb;
    DWORD dwProviderSubType;
    DWORD dwProviderOffset;
    DWORD dwProviderSize;
    WCHAR wcProviderData[1];
} COMMCONFIG,*LPCOMMCONFIG;
#endif // INCL_OS2MM
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

#define SIZE_OF_80387_REGISTERS      80

typedef struct _FLOATING_SAVE_AREA
{
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA, *PFLOATING_SAVE_AREA;

typedef struct _CONTEXT86
{
    DWORD   ContextFlags;

    /* These are selected by CONTEXT_DEBUG_REGISTERS */
    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    /* These are selected by CONTEXT_FLOATING_POINT */
    FLOATING_SAVE_AREA FloatSave;

    /* These are selected by CONTEXT_SEGMENTS */
    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    /* These are selected by CONTEXT_INTEGER */
    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    /* These are selected by CONTEXT_CONTROL */
    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;
    DWORD   EFlags;
    DWORD   Esp;
    DWORD   SegSs;
} CONTEXT;

typedef CONTEXT *PCONTEXT;

#pragma pack()

#endif //__WINE_WINNT_H

typedef struct _UNICODE_STRING {
	USHORT	Length;		/* bytes */
	USHORT	MaximumLength;	/* bytes */
	PWSTR	Buffer;
} UNICODE_STRING,*PUNICODE_STRING;

typedef DWORD COLORREF, *LPCOLORREF;
typedef BOOL (* CALLBACK ABORTPROC)(HDC32, INT);
typedef DWORD HMETAFILE;
typedef DWORD HENHMETAFILE;

#pragma pack()

#endif
