/* $Id: oslibmem.h,v 1.3 2003-01-05 12:31:24 sandervl Exp $ */
/*
 * OS/2 Memory management procedures
 *
 * Copyright 2002 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 */
#ifndef __OSLIBMEM_H__
#define __OSLIBMISC_H__

#ifdef __cplusplus
extern "C" {
#endif

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

#ifndef PAGE_SIZE
#define PAGE_SIZE           4096
#endif

#define MEM_TILED_CEILING   0x1fffffff
               
DWORD OSLibDosAliasMem(LPVOID pb, ULONG cb, LPVOID *ppbAlias, ULONG fl);
DWORD OSLibDosAllocMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags, BOOL fLowMemory = FALSE);
DWORD OSLibDosFreeMem(LPVOID lpMemAddr);
DWORD OSLibDosQueryMem(LPVOID lpMemAddr, DWORD *lpRangeSize, DWORD *lpAttr);
DWORD OSLibDosSetMem(LPVOID lpMemAddr, DWORD size, DWORD flags);
DWORD OSLibDosAllocSharedMem(LPVOID *lplpMemAddr, DWORD size, DWORD flags, LPSTR name);
DWORD OSLibDosGetNamedSharedMem(LPVOID *lplpMemAddr, LPSTR name);
PVOID OSLibDosFindMemBase(LPVOID lpMemAddr, DWORD *lpAttr);

#ifdef __cplusplus
}
#endif

#endif
