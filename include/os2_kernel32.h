/* $Id: os2_kernel32.h,v 1.2 1999-05-27 15:17:58 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * Headfile prototyping all OS2 prefixed kernel32 functions.
 * Internal use.
 * Try to keep alphabetical order.
 *
 * 1998/08/30 KSO knut st. osmundsen
 *
 * @(#) OS2_KERNEL32.H    0.0.1	1998/08/30 KSO created and added some file I/O prototypes
 */


#ifndef _os2_kernel_h_
#define _os2_kernel_h_

BOOL WIN32API OS2CloseHandle(HANDLE hObject);
HANDLE WIN32API OS2CreateFileA(LPCTSTR lpszName,
                               DWORD  fdwAccess,
                               DWORD  fdwShareMode,
                               LPSECURITY_ATTRIBUTES lpsa,
                               DWORD  fdwCreate,
                               DWORD  fdwAttrsAndFlags,
                               HANDLE hTemplateFile);
BOOL WIN32API OS2ReadFile( HANDLE arg1, PVOID arg2, DWORD arg3, PDWORD arg4, POVERLAPPED  arg5);
BOOL WIN32API OS2SetFilePointer(HANDLE hFile,
                                LONG   lDistanceToMove,
                                PLONG  lpDistanceToMoveHigh,
                                DWORD  dwMoveMethod);
BOOL WIN32API OS2WriteFile(HANDLE  hFile,
                           LPCVOID buffer,
                           DWORD   nrbytes,
                           LPDWORD nrbyteswritten,
                           LPOVERLAPPED lpOverLapped);




#endif
