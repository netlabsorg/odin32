/*
 * Internal functions exported by odin dlls
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __ODINAPI_H__
#define __ODINAPI_H__

/** @group kernel32_hm  HandleManager Interfaces
 * @{ */
#ifndef HMTYPE_UNKNOWN
/* these are a duplication of parts in hmhandle.h */
#define HMTYPE_UNKNOWN          0
#define HMTYPE_MEMMAP           1
#define HMTYPE_DEVICE           2
#define HMTYPE_PROCESSTOKEN     3
#define HMTYPE_THREADTOKEN      4
#define HMTYPE_THREAD           5
#define HMTYPE_PIPE             6
#define HMTYPE_EVENTSEM         7
#define HMTYPE_MUTEXSEM         8
#define HMTYPE_SEMAPHORE        9
#define HMTYPE_COMPORT          10
#define HMTYPE_PARPORT          11
#define HMTYPE_MAILSLOT         12
#define HMTYPE_BAD_HANDLE       -1
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Gets the type of an object.
 *
 * @returns the value of one of the HMMTYPE_ defines.
 * @returns HMTYPE_BAD_HANDLE if hObject isn't a valid handle.
 * @param   hObject     Handle to object.
 */
unsigned WIN32API HMQueryObjectType(HANDLE hObject);

#ifdef __cplusplus
} // extern "C"
#endif

/** @} */

#endif //__WINUSER32_H__
