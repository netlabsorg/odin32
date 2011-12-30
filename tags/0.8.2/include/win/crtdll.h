/* $Id: crtdll.h,v 1.6 2000-05-19 12:08:35 sandervl Exp $ */


#ifndef __WINE_CRTDLL_H
#define __WINE_CRTDLL_H

#define CRTDLL_LC_ALL		0
#define CRTDLL_LC_COLLATE	1
#define CRTDLL_LC_CTYPE		2
#define CRTDLL_LC_MONETARY	3
#define CRTDLL_LC_NUMERIC	4
#define CRTDLL_LC_TIME		5
#define CRTDLL_LC_MIN		LC_ALL
#define CRTDLL_LC_MAX		LC_TIME

/* ctype defines */
#define CRTDLL_UPPER		0x1
#define CRTDLL_LOWER		0x2
#define CRTDLL_DIGIT		0x4
#define CRTDLL_SPACE		0x8
#define CRTDLL_PUNCT		0x10
#define CRTDLL_CONTROL		0x20
#define CRTDLL_BLANK		0x40
#define CRTDLL_HEX		0x80
#define CRTDLL_LEADBYTE		0x8000
#define CRTDLL_ALPHA		(0x0100|CRTDLL_UPPER|CRTDLL_LOWER)

/* syserr defines */
const char __syserr00[] = "No Error";
const char __syserr01[] = "Input to function out of range (EDOM)";
const char __syserr02[] = "Output of function out of range (ERANGE)";
const char __syserr03[] = "Argument list too long (E2BIG)";
const char __syserr04[] = "Permission denied (EACCES)";
const char __syserr05[] = "Resource temporarily unavailable (EAGAIN)";
const char __syserr06[] = "Bad file descriptor (EBADF)";
const char __syserr07[] = "Resource busy (EBUSY)";
const char __syserr08[] = "No child processes (ECHILD)";
const char __syserr09[] = "Resource deadlock avoided (EDEADLK)";
const char __syserr10[] = "File exists (EEXIST)";
const char __syserr11[] = "Bad address (EFAULT)";
const char __syserr12[] = "File too large (EFBIG)";
const char __syserr13[] = "Interrupted system call (EINTR)";
const char __syserr14[] = "Invalid argument (EINVAL)";
const char __syserr15[] = "Input or output error (EIO)";
const char __syserr16[] = "Is a directory (EISDIR)";
const char __syserr17[] = "Too many open files (EMFILE)";
const char __syserr18[] = "Too many links (EMLINK)";
const char __syserr19[] = "File name too long (ENAMETOOLONG)";
const char __syserr20[] = "Too many open files in system (ENFILE)";
const char __syserr21[] = "No such device (ENODEV)";
const char __syserr22[] = "No such file or directory (ENOENT)";
const char __syserr23[] = "Unable to execute file (ENOEXEC)";
const char __syserr24[] = "No locks available (ENOLCK)";
const char __syserr25[] = "Not enough memory (ENOMEM)";
const char __syserr26[] = "No space left on drive (ENOSPC)";
const char __syserr27[] = "Function not implemented (ENOSYS)";
const char __syserr28[] = "Not a directory (ENOTDIR)";
const char __syserr29[] = "Directory not empty (ENOTEMPTY)";
const char __syserr30[] = "Inappropriate I/O control operation (ENOTTY)";
const char __syserr31[] = "No such device or address (ENXIO)";
const char __syserr32[] = "Operation not permitted (EPERM)";
const char __syserr33[] = "Broken pipe (EPIPE)";
const char __syserr34[] = "Read-only file system (EROFS)";
const char __syserr35[] = "Invalid seek (ESPIPE)";
const char __syserr36[] = "No such process (ESRCH)";
const char __syserr37[] = "Improper link (EXDEV)";
const char __syserr38[] = "No more files (ENMFILE)";

/* function prototypes used in crtdll.c */
extern int LastErrorToErrno(DWORD);

void * __cdecl CRTDLL_malloc( DWORD size );
void   __cdecl CRTDLL_free( void *ptr );

LPSTR  __cdecl CRTDLL__mbsinc( LPCSTR str );
INT    __cdecl CRTDLL__mbslen( LPCSTR str );
LPWSTR __cdecl CRTDLL__wcsdup( LPCWSTR str );
INT    __cdecl CRTDLL__wcsicmp( LPCWSTR str1, LPCWSTR str2 );
INT    __cdecl CRTDLL__wcsicoll( LPCWSTR str1, LPCWSTR str2 );
LPWSTR __cdecl CRTDLL__wcslwr( LPWSTR str );
INT    __cdecl CRTDLL__wcsnicmp( LPCWSTR str1, LPCWSTR str2, INT n );
LPWSTR __cdecl CRTDLL__wcsnset( LPWSTR str, WCHAR c, INT n );
LPWSTR __cdecl CRTDLL__wcsrev( LPWSTR str );
LPWSTR __cdecl CRTDLL__wcsset( LPWSTR str, WCHAR c );
LPWSTR __cdecl CRTDLL__wcsupr( LPWSTR str );
INT    __cdecl CRTDLL_mbstowcs( LPWSTR dst, LPCSTR src, INT n );
INT    __cdecl CRTDLL_mbtowc( WCHAR *dst, LPCSTR str, INT n );
WCHAR  __cdecl CRTDLL_towlower( WCHAR ch );
WCHAR  __cdecl CRTDLL_towupper( WCHAR ch );
LPWSTR __cdecl CRTDLL_wcscat( LPWSTR dst, LPCWSTR src );
LPWSTR __cdecl CRTDLL_wcschr( LPCWSTR str, WCHAR ch );
INT    __cdecl CRTDLL_wcscmp( LPCWSTR str1, LPCWSTR str2 );
DWORD  __cdecl CRTDLL_wcscoll( LPCWSTR str1, LPCWSTR str2 );
LPWSTR __cdecl CRTDLL_wcscpy( LPWSTR dst, LPCWSTR src );
INT    __cdecl CRTDLL_wcscspn( LPCWSTR str, LPCWSTR reject );
INT    __cdecl CRTDLL_wcslen( LPCWSTR str );
LPWSTR __cdecl CRTDLL_wcsncat( LPWSTR s1, LPCWSTR s2, INT n );
INT    __cdecl CRTDLL_wcsncmp( LPCWSTR str1, LPCWSTR str2, INT n );
LPWSTR __cdecl CRTDLL_wcsncpy( LPWSTR s1, LPCWSTR s2, INT n );
LPWSTR __cdecl CRTDLL_wcspbrk( LPCWSTR str, LPCWSTR accept );
LPWSTR __cdecl CRTDLL_wcsrchr( LPWSTR str, WCHAR ch );
INT    __cdecl CRTDLL_wcsspn( LPCWSTR str, LPCWSTR accept );
LPWSTR __cdecl CRTDLL_wcsstr( LPCWSTR str, LPCWSTR sub );
LPWSTR __cdecl CRTDLL_wcstok( LPWSTR str, LPCWSTR delim );
INT    __cdecl CRTDLL_wcstombs( LPSTR dst, LPCWSTR src, INT n );
INT    __cdecl CRTDLL_wctomb( LPSTR dst, WCHAR ch );

#endif /* __WINE_CRTDLL_H */
