/* $Id: oslibdos.h,v 1.10 2000-01-02 22:51:12 sandervl Exp $ */

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


DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl);
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags);
DWORD OSLibDosFreeMem(LPVOID lpMemAddr);
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr);
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags);
DWORD OSLibDosAllocSharedMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags, LPSTR name);
DWORD OSLibDosGetNamedSharedMem(LPVOID *lplpMemAddr, LPSTR name);

BOOL OSLibDosGetFileAttributesEx(LPSTR pszName, ULONG ulDummy, PVOID pBuffer);

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

#define OSLIB_SEARCHDIR		1
#define OSLIB_SEARCHCURDIR	2
#define OSLIB_SEARCHFILE	3
#define OSLIB_SEARCHENV		4

DWORD OSLibDosSearchPath(DWORD cmd, char *path, char *name, char *full_name, DWORD length_fullname);


DWORD OSLibDosCreate(CHAR *lpFileName,
                     DWORD dwAccess,
                     DWORD dwShare,
                     LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                     DWORD dwCreation,
                     DWORD dwFlags,
                     HANDLE hTemplate,
                     DWORD *dwFile);

DWORD OSLibDosResetBuffer(DWORD hFile);
DWORD OSLibDosDupHandle(DWORD hFile, DWORD *hNew);
DWORD OSLibDosSetFilePtr2(DWORD hFile, DWORD offset, DWORD method);

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

void OSLibDosDisableHardError(BOOL fTurnOff);
BOOL OSLibDosQueryProcTimes(DWORD procid, ULONG *kerneltime, ULONG *usertime);

BOOL OSLibDosTransactNamedPipe( DWORD  hNamedPipe,
                                LPVOID lpInBuffer,
                                DWORD nInBufferSize,
                                LPVOID lpOutBuffer,
                                DWORD nOutBufferSize,
                                LPOVERLAPPED lpOverlapped );
BOOL OSLibDosCallNamedPipe( LPCTSTR lpNamedPipeName,
                         LPVOID  lpInBuffer,
                         DWORD   nInBufferSize,
                         LPVOID  lpOutBuffer,
                         DWORD   nOutBufferSize,
                         LPDWORD lpBytesRead,
                         DWORD   nTimeOut );

BOOL OSLibDosConnectNamedPipe(DWORD hNamedPipe, LPOVERLAPPED lpOverlapped);

DWORD OSLibDosCreateNamedPipe(LPCTSTR lpName, 
                              DWORD   dwOpenMode, 
                              DWORD   dwPipeMode,
                              DWORD   nMaxInstances, 
                              DWORD   nOutBufferSize,
                              DWORD   nInBufferSize, 
                              DWORD   nDefaultTimeOut,
                              void* lpSecurityAttributes);


#endif
