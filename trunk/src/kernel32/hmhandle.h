/* $Id: hmhandle.h,v 1.2 2004/03/12 18:19:41 sandervl Exp $ */

#ifndef __HMHANDLE_H__
#define __HMHANDLE_H__

#include "hmdevice.h"

/*****************************************************************************
 * defines                                                                   *
 *****************************************************************************/
/* note: the defines are duplicated in odinapi.h */
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
//.....


typedef struct _HMHANDLE
{
  HMDeviceHandler *pDeviceHandler;         /* handler for this pseudo-device */
  HMHANDLEDATA    hmHandleData;                  /* attributes of the handle */
} HMHANDLE, *PHMHANDLE;


#ifdef __cplusplus
extern "C" {
#endif

PHMHANDLE     HMHandleQueryPtr(HANDLE hHandle);
PHMHANDLE     HMHandleGetFreePtr(DWORD dwType);
PHMHANDLEDATA HMQueryHandleData(HANDLE handle);

#ifdef __cplusplus
} // extern "C"
#endif


#endif
