/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * PELDR main exe loader header file
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __PE_H__
#define __PE_H__

extern char szErrorTitle[];
extern char szMemErrorMsg[];
extern char szFileErrorMsg[];
extern char szPEErrorMsg[];
extern char szCPUErrorMsg[];
extern char szExeErrorMsg[];
extern char szInteralErrorMsg[];

#define DWORD  ULONG
#define LPVOID VOID *
#define WORD   USHORT 
#define UNALIGNED
#define PDWORD DWORD *
#define PWORD  WORD *
#define NTAPI  
#define BOOLEAN ULONG
#define WCHAR   USHORT
#define HANDLE  ULONG

#define WIN32API __stdcall

#endif // NOIMAGE

