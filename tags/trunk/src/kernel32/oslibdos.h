/* $Id: oslibdos.h,v 1.2 1999-08-26 15:05:14 sandervl Exp $ */

/*
 * Wrappers for OS/2 Dos* API
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OSLIBDOS_H__
#define __OSLIBDOS_H__


DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags);
DWORD OSLibDosFreeMem(LPVOID lpMemAddr);
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr);
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags);

#define OSLIB_NOERROR			0
#define OSLIB_ERROR_INVALID_ADDRESS	1
#define OSLIB_ERROR_ACCESS_DENIED       2
#define OSLIB_ERROR_INVALID_PARAMETER   3

#ifndef __OS2_H__

/* Access protection                                                          */
#define PAG_READ          0x00000001U      /* read access                      */
#define PAG_WRITE         0x00000002U      /* write access                     */
#define PAG_EXECUTE       0x00000004U      /* execute access                   */
#define PAG_GUARD         0x00000008U      /* guard protection                 */
#define PAG_DEFAULT       0x00000400U      /* default (initial) access         */

/* Commit                                                                     */
#define PAG_COMMIT        0x00000010U      /* commit storage                   */
#define PAG_DECOMMIT      0x00000020U      /* decommit storage                 */

/* Allocation attributes                                                      */
#define OBJ_TILE          0x00000040U      /* tile object                      */
#define OBJ_PROTECTED     0x00000080U      /* protect object */
#define OBJ_GETTABLE      0x00000100U      /* gettable by other processes      */
#define OBJ_GIVEABLE      0x00000200U      /* giveable to other processes      */

/* Allocation type (returned from DosQueryMem)                                */
#define PAG_SHARED        0x00002000U     /* shared object                    */
#define PAG_FREE          0x00004000U     /* pages are free                   */
#define PAG_BASE          0x00010000U     /* first page in object             */

#endif

#define OSLIB_ACCESS_READONLY		1
#define OSLIB_ACCESS_READWRITE		2
#define OSLIB_ACCESS_SHAREDENYNONE	4
#define OSLIB_ACCESS_SHAREDENYREAD	8
#define OSLIB_ACCESS_SHAREDENYWRITE	16

DWORD OSLibDosOpen(char *lpszFileName, DWORD flags);
DWORD OSLibDosClose(DWORD hFile);
DWORD OSLibDosGetFileSize(DWORD hFile);
DWORD OSLibDosRead(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesRead);
DWORD OSLibDosWrite(DWORD hFile, LPVOID lpBuffer, DWORD size, DWORD *nrBytesWritten);

#define OSLIB_SETPTR_FILE_CURRENT	1
#define OSLIB_SETPTR_FILE_BEGIN		2
#define OSLIB_SETPTR_FILE_END		3

DWORD OSLibDosSetFilePtr(DWORD hFile, DWORD offset, DWORD method);

#endif
