/*
 * Basic types definitions
 *
 * Copyright 1996 Alexandre Julliard
 */

#ifndef __WINE_WINDEF_H
#define __WINE_WINDEF_H

#if (defined(__WINE__)) && (!defined(__WIN32OS2__))
# include "config.h"
# undef UNICODE
#endif  /* __WINE__ */

#ifdef __WIN32OS2__
#if defined (__IBMC__) || (defined(__IBMCPP__) && (__IBMCPP__ < 400))
#define CINTERFACE
#endif
#endif

#define WINVER 0x0500

#ifdef UNICODE
#include <wchar.h>
#endif

#include <basetsd.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Misc. constants. */

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

#ifdef TRUE
#undef TRUE
#endif
#define TRUE  1

#ifdef NULL
#undef NULL
#endif
#define NULL  0

/* Macros to map Winelib names to the correct implementation name */
/* depending on __WINE__ and UNICODE macros.                      */
/* Note that Winelib is purely Win32.                             */

#ifdef __WINE__
#ifdef __WIN32OS2__
# define WINELIB_NAME_AW(func)   func
#else
# define WINELIB_NAME_AW(func)   this_is_a_syntax_error this_is_a_syntax_error
#endif
#else  /* __WINE__ */
# ifdef UNICODE
#  define WINELIB_NAME_AW(func) func##W
# else
#  define WINELIB_NAME_AW(func) func##A
# endif  /* UNICODE */
#endif  /* __WINE__ */

#ifdef __WINE__
# define DECL_WINELIB_TYPE_AW(type)  /* nothing */
#else   /* __WINE__ */
# define DECL_WINELIB_TYPE_AW(type)  typedef WINELIB_NAME_AW(type) type;
#endif  /* __WINE__ */


/* Calling conventions definitions */

#ifndef __WIN32OS2__
#ifdef __i386__
# if defined(__GNUC__) && (__GNUC__ == 2) && (__GNUC_MINOR__ >= 7)
#  define __stdcall __attribute__((__stdcall__))
#  define __cdecl   __attribute__((__cdecl__))
#  define __RESTORE_ES  __asm__ __volatile__("pushl %ds\n\tpopl %es")
# else
#  error You need gcc >= 2.7 to build Wine on a 386
# endif  /* __GNUC__ */
#else  /* __i386__ */
# define __stdcall
# define __cdecl
# define __RESTORE_ES
#endif  /* __i386__ */
#endif


#ifdef __WIN32OS2__
#include <odin.h>
#if defined(__GNUC__)
extern int _argcA;
extern char **_argvA;
#define __argcA _argcA
#define __argvA _argvA
#else
extern int __argcA;
extern char **__argvA;
#endif
#endif

/* define needed macros as required */
#ifndef CALLBACK
  #define CALLBACK    __stdcall
#endif

#ifndef WINAPI
  #define WINAPI      __stdcall
#endif

#ifndef APIPRIVATE
  #define APIPRIVATE  __stdcall
#endif

#ifndef PASCAL
  #define PASCAL      __stdcall
#endif

#ifndef pascal
  #define pascal      __stdcall
#endif

#ifndef _pascal
  #define _pascal     __stdcall
#endif

#ifndef _stdcall
  #define _stdcall    __stdcall
#endif

#ifndef _fastcall
  #define _fastcall   __stdcall
#endif

#ifndef __export
  #define __export
#endif

#ifndef CDECL
  #define CDECL       __cdecl
#endif

#ifndef _CDECL
  #define _CDECL      __cdecl
#endif

#ifndef cdecl
  #define cdecl       __cdecl
#endif

#ifndef _cdecl
  #define _cdecl      __cdecl
#endif

#ifndef WINAPIV
  #define WINAPIV     __cdecl
#endif

#ifndef APIENTRY
  #define APIENTRY    WINAPI
#endif

#ifndef WIN32API
  #define WIN32API    WINAPI
#endif

#ifndef __EMX__
#if (__IBMC__ < 400) && (__IBMCPP__ < 360)
#ifndef __declspec
  #define __declspec(x)
#endif

#ifndef dllimport
  #define dllimport
#endif

#ifndef dllexport
  #define dllexport
#endif
#endif // (__IBMC__ < 400) && (__IBMCPP__ < 360)
#endif // __EMX__

#ifndef CONST
  #define CONST       const
#endif

/* Anonymous union/struct handling */

#ifndef NONAMELESSSTRUCT
#define DUMMYSTRUCTNAME
#define DUMMYSTRUCTNAME1
#define DUMMYSTRUCTNAME2
#define DUMMYSTRUCTNAME3
#define DUMMYSTRUCTNAME4
#define DUMMYSTRUCTNAME5
#define DUMMYSTRUCTNAME_DOT
#define DUMMYSTRUCTNAME1_DOT
#define DUMMYSTRUCTNAME2_DOT
#define DUMMYSTRUCTNAME3_DOT
#define DUMMYSTRUCTNAME4_DOT
#define DUMMYSTRUCTNAME5_DOT
#else /* !defined(NONAMELESSSTRUCT) */
#define DUMMYSTRUCTNAME   s
#define DUMMYSTRUCTNAME1  s1
#define DUMMYSTRUCTNAME2  s2
#define DUMMYSTRUCTNAME3  s3
#define DUMMYSTRUCTNAME4  s4
#define DUMMYSTRUCTNAME5  s5
#define DUMMYSTRUCTNAME_DOT   s.
#define DUMMYSTRUCTNAME1_DOT  s1.
#define DUMMYSTRUCTNAME2_DOT  s2.
#define DUMMYSTRUCTNAME3_DOT  s3.
#define DUMMYSTRUCTNAME4_DOT  s4.
#define DUMMYSTRUCTNAME5_DOT  s5.
#endif /* !defined(NONAMELESSSTRUCT) */

#ifndef NONAMELESSUNION
#define DUMMYUNIONNAME
#define DUMMYUNIONNAME1
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#define DUMMYUNIONNAME4
#define DUMMYUNIONNAME5
#define DUMMYUNIONNAME6
#define DUMMYUNIONNAME7
#define DUMMYUNIONNAME8
#define DUMMYUNIONNAME_DOT
#define DUMMYUNIONNAME1_DOT
#define DUMMYUNIONNAME2_DOT
#define DUMMYUNIONNAME3_DOT
#define DUMMYUNIONNAME4_DOT
#define DUMMYUNIONNAME5_DOT
#define DUMMYUNIONNAME6_DOT
#define DUMMYUNIONNAME7_DOT
#define DUMMYUNIONNAME8_DOT
#else /* !defined(NONAMELESSUNION) */
#define DUMMYUNIONNAME   u
#define DUMMYUNIONNAME1  u1
#define DUMMYUNIONNAME2  u2
#define DUMMYUNIONNAME3  u3
#define DUMMYUNIONNAME4  u4
#define DUMMYUNIONNAME5  u5
#define DUMMYUNIONNAME6  u6
#define DUMMYUNIONNAME7  u7
#define DUMMYUNIONNAME8  u8
#define DUMMYUNIONNAME_DOT   u.
#define DUMMYUNIONNAME1_DOT  u1.
#define DUMMYUNIONNAME2_DOT  u2.
#define DUMMYUNIONNAME3_DOT  u3.
#define DUMMYUNIONNAME4_DOT  u4.
#define DUMMYUNIONNAME5_DOT  u5.
#define DUMMYUNIONNAME6_DOT  u6.
#define DUMMYUNIONNAME7_DOT  u7.
#define DUMMYUNIONNAME8_DOT  u8.
#endif /* !defined(NONAMELESSUNION) */


/* Standard data types. These are the same for emulator and library. */

#if defined(__GNUC__) && defined(__cplusplus)
/* prototypes like foo(VOID) don't work with typedef */
#define VOID            void
#else
typedef void            VOID;
#endif
typedef int             INT;
typedef unsigned int    UINT;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef unsigned long   ULONG;
typedef unsigned char   BYTE;
typedef long            LONG;
typedef short           SHORT;
typedef unsigned short  USHORT;
typedef char            CHAR;
typedef unsigned char   UCHAR;
/* Some systems might have wchar_t, but we really need 16 bit characters */
#ifdef UNICODE
typedef wchar_t         WCHAR;
#else
typedef unsigned short  WCHAR;
#endif
typedef unsigned short  BOOL16;
#ifdef __FORCE_BOOL_AS_INT__
typedef int             BOOL;
#else
typedef unsigned long   BOOL;
#endif
typedef double          DATE;
typedef double          DOUBLE;
#if (__IBMC__ < 360) && (__IBMCPP__ < 360) && !defined (__EMX__)
typedef double          LONGLONG;
typedef double          ULONGLONG;
typedef double          ULONGULONG;
typedef double          DWORDLONG;
#elif defined (__EMX__)
#ifndef _INT64_T_DECLARED
typedef	long long		  int64_t; /* LONGLONG */
typedef	unsigned long long	u_int64_t; /* LONGLONG */
#endif
#ifndef _UINT64_T_DECLARED
typedef	unsigned long long	 uint64_t; /* LONGLONG */
#endif
typedef int64_t         LONGLONG;
typedef uint64_t        ULONGLONG;
typedef uint64_t        ULONGULONG;
typedef uint64_t        DWORDLONG;
#else
#include <inttypes.h>
typedef int64_t         LONGLONG;
typedef uint64_t        ULONGLONG;
typedef uint64_t        ULONGULONG;
typedef uint64_t        DWORDLONG;
#endif

/* FIXME: Wine does not compile with strict on, therefore strict
 * handles are presently only usable on machines where sizeof(UINT) ==
 * sizeof(void*).  HANDLEs are supposed to be void* but a large amount
 * of WINE code operates on HANDLES as if they are UINTs. So to WINE
 * they exist as UINTs but to the Winelib user who turns on strict,
 * they exist as void*. If there is a size difference between UINT and
 * void* then things get ugly.  */
#ifdef STRICT
typedef UINT16          HANDLE16;
typedef VOID*           HANDLE;
#else
typedef UINT16          HANDLE16;
typedef ULONG           HANDLE;
#endif

typedef HANDLE16       *LPHANDLE16;
typedef HANDLE         *LPHANDLE;
typedef HANDLE         GLOBALHANDLE;
typedef HANDLE         LOCALHANDLE;

/* Integer types. These are the same for emulator and library. */
typedef UINT16          WPARAM16;
typedef UINT            WPARAM;
typedef LONG            LPARAM;
typedef LONG            HRESULT;
typedef LONG            LRESULT;
typedef WORD            ATOM;
typedef WORD            CATCHBUF[9];
typedef WORD           *LPCATCHBUF;
typedef DWORD           REGSAM;
typedef HANDLE          HHOOK;
typedef HANDLE          HKEY;
typedef HANDLE          HMONITOR;
typedef DWORD           LCID;
typedef WORD            LANGID;
typedef DWORD           LCTYPE;
typedef float           FLOAT;

/* Pointers types. These are the same for emulator and library. */
/* winnt types */
typedef VOID           *PVOID;
typedef const void     *PCVOID;
typedef CHAR           *PCHAR;
typedef UCHAR          *PUCHAR;
typedef BYTE           *PBYTE;
typedef USHORT         *PUSHORT;
typedef SHORT          *PSHORT;
typedef ULONG          *PULONG;
typedef LONG           *PLONG;
typedef DWORD          *PDWORD;
/* common win32 types */
#ifdef __WINE__
typedef CHAR           *LPTSTR;
typedef const CHAR     *LPCTSTR;
#endif
typedef CHAR           *LPSTR;
typedef CHAR           *PSTR;
typedef const CHAR     *LPCSTR;
typedef const CHAR     *PCSTR;
typedef WCHAR          *LPWSTR;
typedef WCHAR          *PWSTR;
typedef const WCHAR    *LPCWSTR;
typedef const WCHAR    *PCWSTR;
typedef BYTE           *LPBYTE;
typedef WORD           *LPWORD;
typedef DWORD          *LPDWORD;
typedef LONG           *LPLONG;
typedef VOID           *LPVOID;
typedef const VOID     *LPCVOID;
typedef INT16          *LPINT16;
typedef UINT16         *LPUINT16;
typedef INT            *PINT;
typedef INT            *LPINT;
typedef UINT           *PUINT;
typedef UINT           *LPUINT;
typedef HKEY           *LPHKEY;
typedef HKEY           *PHKEY;
typedef FLOAT          *PFLOAT;
typedef FLOAT          *LPFLOAT;
typedef BOOL           *PBOOL;
typedef BOOL           *LPBOOL;

/* Special case: a segmented pointer is just a pointer in the user's code. */

#ifdef __WINE__
typedef DWORD SEGPTR;
#else
typedef void* SEGPTR;
#endif /* __WINE__ */

/* Handle types that exist both in Win16 and Win32. */

#ifdef STRICT
#define DECLARE_HANDLE(a) \
	typedef HANDLE16 a##16; \
	typedef a##16 *P##a##16; \
	typedef a##16 *NP##a##16; \
	typedef a##16 *LP##a##16; \
	typedef struct a##__ { int unused; } *a; \
	typedef a *P##a; \
	typedef a *LP##a
#else /*STRICT*/
#define DECLARE_HANDLE(a) \
	typedef HANDLE16 a##16; \
	typedef a##16 *P##a##16; \
	typedef a##16 *NP##a##16; \
	typedef a##16 *LP##a##16; \
	typedef HANDLE a; \
	typedef a *P##a; \
	typedef a *LP##a
#endif /*STRICT*/

DECLARE_HANDLE(HACMDRIVERID);
DECLARE_HANDLE(HACMDRIVER);
DECLARE_HANDLE(HACMOBJ);
DECLARE_HANDLE(HACMSTREAM);
DECLARE_HANDLE(HMETAFILEPICT);

typedef int HFILE;
typedef HANDLE16 HFILE16;

DECLARE_HANDLE(HACCEL);
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HBRUSH);
DECLARE_HANDLE(HCOLORSPACE);
DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HGLRC);          // OpenGL
DECLARE_HANDLE(HDROP);
DECLARE_HANDLE(HDRVR);
DECLARE_HANDLE(HDWP);
DECLARE_HANDLE(HENHMETAFILE);
DECLARE_HANDLE(HFONT);
DECLARE_HANDLE(HICON);
DECLARE_HANDLE(HINSTANCE);
DECLARE_HANDLE(HMENU);
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HMIDI);
DECLARE_HANDLE(HMIDIIN);
DECLARE_HANDLE(HMIDIOUT);
DECLARE_HANDLE(HMIDISTRM);
DECLARE_HANDLE(HMIXER);
DECLARE_HANDLE(HMIXEROBJ);
DECLARE_HANDLE(HMMIO);
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HPEN);
DECLARE_HANDLE(HQUEUE);
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HWAVE);
DECLARE_HANDLE(HWAVEIN);
DECLARE_HANDLE(HWAVEOUT);
DECLARE_HANDLE(HWINSTA);
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(HWND);
DECLARE_HANDLE(HKL);
DECLARE_HANDLE(HIC);
DECLARE_HANDLE(HRASCONN);
DECLARE_HANDLE(HPRINTER);

/* HICONs & HCURSORs are polymorphic */
typedef HICON HCURSOR;
typedef HICON16 HCURSOR16;

/* Handle types that must remain interchangeable even with strict on */

typedef HINSTANCE16 HMODULE16;
typedef HINSTANCE HMODULE;
typedef HANDLE16 HGDIOBJ16;
typedef HANDLE16 HGLOBAL16;
typedef HANDLE16 HLOCAL16;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HLOCAL;

/* Callback function pointers types */

#ifdef __IBMC__
typedef BOOL16  (* CALLBACK DLGPROC16)(HWND16,UINT16,WPARAM16,LPARAM);
typedef BOOL    (* CALLBACK DLGPROC)(HWND,UINT,WPARAM,LPARAM);
typedef LRESULT (* CALLBACK DRIVERPROC16)(DWORD,HDRVR16,UINT16,LPARAM,LPARAM);
typedef LRESULT (* CALLBACK DRIVERPROC)(DWORD,HDRVR,UINT,LPARAM,LPARAM);
typedef INT16   (* CALLBACK EDITWORDBREAKPROC16)(LPSTR,INT16,INT16,INT16);
typedef INT     (* CALLBACK EDITWORDBREAKPROCA)(LPSTR,INT,INT,INT);
typedef INT     (* CALLBACK EDITWORDBREAKPROCW)(LPWSTR,INT,INT,INT);
DECL_WINELIB_TYPE_AW(EDITWORDBREAKPROC)
typedef LRESULT (* CALLBACK FARPROC16)();
typedef LRESULT (* CALLBACK FARPROC)();
typedef INT16   (* CALLBACK PROC16)();
typedef INT     (* CALLBACK PROC)();
typedef INT16   (* CALLBACK GOBJENUMPROC16)(SEGPTR,LPARAM);
typedef INT     (* CALLBACK GOBJENUMPROC)(LPVOID,LPARAM);
typedef BOOL16  (* CALLBACK GRAYSTRINGPROC16)(HDC16,LPARAM,INT16);
typedef BOOL    (* CALLBACK GRAYSTRINGPROC)(HDC,LPARAM,INT);
typedef LRESULT (* CALLBACK HOOKPROC16)(INT16,WPARAM16,LPARAM);
typedef LRESULT (* CALLBACK HOOKPROC)(INT,WPARAM,LPARAM);
typedef VOID    (* CALLBACK LINEDDAPROC16)(INT16,INT16,LPARAM);
typedef VOID    (* CALLBACK LINEDDAPROC)(INT,INT,LPARAM);
typedef BOOL16  (* CALLBACK PROPENUMPROC16)(HWND16,SEGPTR,HANDLE16);
typedef BOOL    (* CALLBACK PROPENUMPROCA)(HWND,LPCSTR,HANDLE);
typedef BOOL    (* CALLBACK PROPENUMPROCW)(HWND,LPCWSTR,HANDLE);
DECL_WINELIB_TYPE_AW(PROPENUMPROC)
typedef BOOL    (* CALLBACK PROPENUMPROCEXA)(HWND,LPCSTR,HANDLE,LPARAM);
typedef BOOL    (* CALLBACK PROPENUMPROCEXW)(HWND,LPCWSTR,HANDLE,LPARAM);
DECL_WINELIB_TYPE_AW(PROPENUMPROCEX)
typedef VOID    (* CALLBACK TIMERPROC16)(HWND16,UINT16,UINT16,DWORD);
typedef VOID    (* CALLBACK TIMERPROC)(HWND,UINT,UINT,DWORD);
typedef LRESULT (* CALLBACK WNDENUMPROC16)(HWND16,LPARAM);
//typedef LRESULT (* CALLBACK WNDENUMPROC)(HWND,LPARAM);
typedef BOOL    (* CALLBACK WNDENUMPROC)(HWND,LPARAM);
typedef LRESULT (* CALLBACK WNDPROC16)(HWND16,UINT16,WPARAM16,LPARAM);
typedef LRESULT (* CALLBACK WNDPROC)(HWND,UINT,WPARAM,LPARAM);
#else
typedef BOOL16  ( CALLBACK * DLGPROC16)(HWND16,UINT16,WPARAM16,LPARAM);
typedef BOOL    ( CALLBACK * DLGPROC)(HWND,UINT,WPARAM,LPARAM);
typedef LRESULT ( CALLBACK * DRIVERPROC16)(DWORD,HDRVR16,UINT16,LPARAM,LPARAM);
typedef LRESULT ( CALLBACK * DRIVERPROC)(DWORD,HDRVR,UINT,LPARAM,LPARAM);
typedef INT16   ( CALLBACK * EDITWORDBREAKPROC16)(LPSTR,INT16,INT16,INT16);
typedef INT     ( CALLBACK * EDITWORDBREAKPROCA)(LPSTR,INT,INT,INT);
typedef INT     ( CALLBACK * EDITWORDBREAKPROCW)(LPWSTR,INT,INT,INT);
DECL_WINELIB_TYPE_AW(EDITWORDBREAKPROC)
typedef LRESULT ( CALLBACK * FARPROC16)();
typedef LRESULT ( CALLBACK * FARPROC)();
typedef INT16   ( CALLBACK * PROC16)();
typedef INT     ( CALLBACK * PROC)();
typedef INT16   ( CALLBACK * GOBJENUMPROC16)(SEGPTR,LPARAM);
typedef INT     ( CALLBACK * GOBJENUMPROC)(LPVOID,LPARAM);
typedef BOOL16  ( CALLBACK * GRAYSTRINGPROC16)(HDC16,LPARAM,INT16);
typedef BOOL    ( CALLBACK * GRAYSTRINGPROC)(HDC,LPARAM,INT);
typedef LRESULT ( CALLBACK * HOOKPROC16)(INT16,WPARAM16,LPARAM);
typedef LRESULT ( CALLBACK * HOOKPROC)(INT,WPARAM,LPARAM);
typedef VOID    ( CALLBACK * LINEDDAPROC16)(INT16,INT16,LPARAM);
typedef VOID    ( CALLBACK * LINEDDAPROC)(INT,INT,LPARAM);
typedef BOOL16  ( CALLBACK * PROPENUMPROC16)(HWND16,SEGPTR,HANDLE16);
typedef BOOL    ( CALLBACK * PROPENUMPROCA)(HWND,LPCSTR,HANDLE);
typedef BOOL    ( CALLBACK * PROPENUMPROCW)(HWND,LPCWSTR,HANDLE);
DECL_WINELIB_TYPE_AW(PROPENUMPROC)
typedef BOOL    ( CALLBACK * PROPENUMPROCEXA)(HWND,LPCSTR,HANDLE,LPARAM);
typedef BOOL    ( CALLBACK * PROPENUMPROCEXW)(HWND,LPCWSTR,HANDLE,LPARAM);
DECL_WINELIB_TYPE_AW(PROPENUMPROCEX)
typedef VOID    ( CALLBACK * TIMERPROC16)(HWND16,UINT16,UINT16,DWORD);
typedef VOID    ( CALLBACK * TIMERPROC)(HWND,UINT,UINT,DWORD);
typedef LRESULT ( CALLBACK * WNDENUMPROC16)(HWND16,LPARAM);
typedef BOOL    ( CALLBACK * WNDENUMPROC)(HWND,LPARAM);
typedef LRESULT ( CALLBACK * WNDPROC16)(HWND16,UINT16,WPARAM16,LPARAM);
typedef LRESULT ( CALLBACK * WNDPROC)(HWND,UINT,WPARAM,LPARAM);
#endif

/* TCHAR data types definitions for Winelib. */
/* These types are _not_ defined for the emulator, because they */
/* depend on the UNICODE macro that only exists in user's code. */

#if !defined(__WINE__) && !defined(_TCHAR_DEFINED)
#define _TCHAR_DEFINED
# ifdef UNICODE
typedef WCHAR TCHAR, *PTCHAR;
typedef LPWSTR LPTSTR, PTSTR;
typedef LPCWSTR LPCTSTR, PCTSTR;
#define __TEXT(string) L##string /*probably wrong */
# else  /* UNICODE */
typedef CHAR TCHAR, *PTCHAR;
typedef LPSTR LPTSTR, PTSTR;
typedef LPCSTR LPCTSTR, PCTSTR;
#define __TEXT(string) string
# endif /* UNICODE */
#endif   /* __WINE__ */
#define TEXT(quote) __TEXT(quote)

/* Data types specific to the library. These do _not_ exist in the emulator. */



/* Define some empty macros for compatibility with Windows code. */

#ifndef __WINE__
#define NEAR
#define FAR
#define near
#define far
#define _near
#define _far
#define IN
#define OUT
#define OPTIONAL
#endif  /* __WINE__ */

/* Macro for structure packing. */

#ifdef __GNUC__
#define WINE_PACKED __attribute__ ((packed))
#define WINE_UNUSED __attribute__ ((unused))
#else
#define WINE_PACKED  /* nothing */
#define WINE_UNUSED  /* nothing */
#endif

/* Macros to split words and longs. */

#define LOBYTE(w)              ((BYTE)(WORD)(w))
#define HIBYTE(w)              ((BYTE)((WORD)(w) >> 8))

#define LOWORD(l)              ((WORD)(DWORD)(l))
#define HIWORD(l)              ((WORD)((DWORD)(l) >> 16))

#define SLOWORD(l)             ((INT16)(LONG)(l))
#define SHIWORD(l)             ((INT16)((LONG)(l) >> 16))

#define MAKEWORD(low,high)     ((WORD)(((BYTE)(low)) | ((WORD)((BYTE)(high))) << 8))
#define MAKELONG(low,high)     ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#define MAKELPARAM(low,high)   ((LPARAM)MAKELONG(low,high))
#define MAKEWPARAM(low,high)   ((WPARAM)MAKELONG(low,high))
#define MAKELRESULT(low,high)  ((LRESULT)MAKELONG(low,high))
#define MAKEINTATOM(atom)      ((LPCSTR)MAKELONG((atom),0))

#define SELECTOROF(ptr)     (HIWORD(ptr))
#define OFFSETOF(ptr)       (LOWORD(ptr))

/* Macros to access unaligned or wrong-endian WORDs and DWORDs. */
/* Note: These macros are semantically broken, at least for wrc.  wrc
   spits out data in the platform's current binary format, *not* in
   little-endian format.  These macros are used throughout the resource
   code to load and store data to the resources.  Since it is unlikely
   that we'll ever be dealing with little-endian resource data, the
   byte-swapping nature of these macros has been disabled.  Rather than
   remove the use of these macros from the resource loading code, the
   macros have simply been disabled.  In the future, someone may want
   to reactivate these macros for other purposes.  In that case, the
   resource code will have to be modified to use different macros. */

#if 1
#define PUT_WORD(ptr,w)   (*(WORD *)(ptr) = (w))
#define GET_WORD(ptr)     (*(WORD *)(ptr))
#define PUT_DWORD(ptr,dw) (*(DWORD *)(ptr) = (dw))
#define GET_DWORD(ptr)    (*(DWORD *)(ptr))
#else
#define PUT_WORD(ptr,w)   (*(BYTE *)(ptr) = LOBYTE(w), \
                           *((BYTE *)(ptr) + 1) = HIBYTE(w))
#define GET_WORD(ptr)     ((WORD)(*(BYTE *)(ptr) | \
                                  (WORD)(*((BYTE *)(ptr)+1) << 8)))
#define PUT_DWORD(ptr,dw) (PUT_WORD((ptr),LOWORD(dw)), \
                           PUT_WORD((WORD *)(ptr)+1,HIWORD(dw)))
#define GET_DWORD(ptr)    ((DWORD)(GET_WORD(ptr) | \
                                   ((DWORD)GET_WORD((WORD *)(ptr)+1) << 16)))
#endif  /* 1 */

/* MIN and MAX macros */

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#ifdef __max /* watcom stdlib.h defines this */
#undef __max
#endif
#define __max(a,b) MAX(a,b)

#ifdef __min /* watcom stdlib.h defines this */
#undef __min
#endif
#define __min(a,b) MIN(a,b)
#ifndef RC_INVOKED
#include <stdlib.h>
#endif

#if !defined(__WATCOMC__) || !defined(_MAX_PATH)
#define _MAX_PATH  260
#endif
#define MAX_PATH   260
#define _MAX_DRIVE 3
#if !defined(__WATCOMC__) && !defined(_MAX_DIR)
#define _MAX_DIR   256
#endif
#if !defined(__WIN32OS2__) && !defined(_MAX_FNAME)
#define _MAX_FNAME 255
#endif
#if !defined(__WATCOMC__) && !defined(_MAX_EXT)
#define _MAX_EXT   256
#endif

#define HFILE_ERROR16   ((HFILE16)-1)
#define HFILE_ERROR     ((HFILE)-1)

/* Winelib run-time flag */

#ifdef __WINE__
extern int __winelib;
#endif  /* __WINE__ */

/* The SIZE structure */

typedef struct
{
    INT16  cx;
    INT16  cy;
} SIZE16, *PSIZE16, *LPSIZE16;

typedef struct tagSIZE
{
    INT  cx;
    INT  cy;
} SIZE, *PSIZE, *LPSIZE;


typedef SIZE SIZEL, *PSIZEL, *LPSIZEL;

#define CONV_SIZE16TO32(s16,s32) \
            ((s32)->cx = (INT)(s16)->cx, (s32)->cy = (INT)(s16)->cy)
#define CONV_SIZE32TO16(s32,s16) \
            ((s16)->cx = (INT16)(s32)->cx, (s16)->cy = (INT16)(s32)->cy)

/* The POINT structure */

typedef struct
{
    INT16  x;
    INT16  y;
} POINT16, *PPOINT16, *LPPOINT16;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, *LPPOINT;

typedef struct _POINTL
{
    LONG x;
    LONG y;
} POINTL;

#define CONV_POINT16TO32(p16,p32) \
            ((p32)->x = (INT)(p16)->x, (p32)->y = (INT)(p16)->y)
#define CONV_POINT32TO16(p32,p16) \
            ((p16)->x = (INT16)(p32)->x, (p16)->y = (INT16)(p32)->y)

#define MAKEPOINT(l)  (*((POINT *)&(l)))

#define MAKEPOINT16(l) (*((POINT16 *)&(l)))

/* The POINTS structure */

typedef struct tagPOINTS
{
	SHORT x;
	SHORT y;
} POINTS, *PPOINTS, *LPPOINTS;


#define MAKEPOINTS(l)  (*((POINTS *)&(l)))


/* The RECT structure */

typedef struct
{
    INT16  left;
    INT16  top;
    INT16  right;
    INT16  bottom;
} RECT16, *LPRECT16;

typedef struct tagRECT
{
    INT  left;
    INT  top;
    INT  right;
    INT  bottom;
} RECT, *PRECT, *LPRECT;
typedef const RECT *LPCRECT;


typedef struct tagRECTL
{
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECTL, *PRECTL, *LPRECTL;

typedef const RECTL *LPCRECTL;

/*
 * 8.9.99 DJR DECIMAL support
 */
typedef struct tagDEC
{
    USHORT wReserved;
    union
    {
      struct
      {
          BYTE scale;
          BYTE sign;
      } a;
      USHORT signscale;
    } b;
    ULONG Hi32;
    union
    {
      struct
      {
          ULONG Lo32;
          ULONG Mid32;
      } c;
      ULONGLONG Lo64;
    } d;
} DECIMAL;
typedef DECIMAL *LPDECIMAL;

#include <winbase.h>

#ifdef __cplusplus
}
#endif

#endif /* __WINE_WINDEF_H */
