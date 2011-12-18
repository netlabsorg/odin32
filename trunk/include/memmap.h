#ifndef __MEMMAP_H
#define __MEMMAP_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _DEF_PFNEXCEPTIONNOTIFY
#define _DEF_PFNEXCEPTIONNOTIFY
typedef BOOL (WIN32API * PFNEXCEPTIONNOTIFY)(LPVOID lpBase, ULONG offset, BOOL fWriteAccess, DWORD dwSize, DWORD dwUserData);
#endif

BOOL WIN32API MMAP_RegisterMemoryRange(PFNEXCEPTIONNOTIFY pfnNotify, LPVOID lpViewAddr, DWORD size, DWORD dwUserData);
BOOL WIN32API MMAP_UnregisterMemoryRange(LPVOID lpViewAddr);

#ifdef __cplusplus
} // extern "C"
#endif

#endif  /*__MEMMAP_H */

