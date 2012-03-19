/* $Id: monitor.h,v 1.2 1999-11-27 14:15:42 cbratschi Exp $ */

/*
 * Monitor definitions
 *
 */

#ifndef __WINE_MONITOR_H
#define __WINE_MONITOR_H

//#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tagMONITOR_DRIVER;

typedef struct tagMONITOR
{
  void *pDriverData;
} MONITOR;

typedef struct tagMONITOR_DRIVER {
  void   (*pInitialize)(struct tagMONITOR *);
  void   (*pFinalize)(struct tagMONITOR *);
  BOOL   (*pIsSingleWindow)(struct tagMONITOR *);
  int    (*pGetWidth)(struct tagMONITOR *);
  int    (*pGetHeight)(struct tagMONITOR *);
  int    (*pGetDepth)(struct tagMONITOR *);
  BOOL   (*pGetScreenSaveActive)(struct tagMONITOR *);
  void   (*pSetScreenSaveActive)(struct tagMONITOR *, BOOL);
  int    (*pGetScreenSaveTimeout)(struct tagMONITOR *);
  void   (*pSetScreenSaveTimeout)(struct tagMONITOR *, int);
} MONITOR_DRIVER;

extern MONITOR_DRIVER *MONITOR_Driver;

extern MONITOR MONITOR_PrimaryMonitor;

extern void MONITOR_Initialize(MONITOR *pMonitor);
extern void MONITOR_Finalize(MONITOR *pMonitor);
extern BOOL MONITOR_IsSingleWindow(MONITOR *pMonitor);
extern int MONITOR_GetWidth(MONITOR *pMonitor);
extern int MONITOR_GetHeight(MONITOR *pMonitor);
extern int MONITOR_GetDepth(MONITOR *pMonitor);
extern BOOL MONITOR_GetScreenSaveActive(MONITOR *pMonitor);
extern void MONITOR_SetScreenSaveActive(MONITOR *pMonitor, BOOL bActivate);
extern int MONITOR_GetScreenSaveTimeout(MONITOR *pMonitor);
extern void MONITOR_SetScreenSaveTimeout(MONITOR *pMonitor, int nTimeout);

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* __WINE_MONITOR_H */

